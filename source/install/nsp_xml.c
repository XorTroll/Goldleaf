#include "install/nsp_xml.h"

#include <errno.h>
#include <machine/endian.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "lib/xml.h"

static u8 getTitleTypeCode(char *str)
{
    if (strcmp(str, "Application") == 0)
    {
        return 0x80;
    }
    else if (strcmp(str, "Patch") == 0)
    {
        return 0x81;
    }
    else if (strcmp(str, "AddOnContent") == 0)
    {
        return 0x82;
    }
    else if (strcmp(str, "Delta") == 0)
    {
        return 0x83;
    }

    return 0x0;    
}

static u8 getContentTypeCode(char *str)
{
    if (strcmp(str, "Meta") == 0)
    {
        return 0x0;
    }
    else if (strcmp(str, "Program") == 0)
    {
        return 0x1;
    }
    else if (strcmp(str, "Data") == 0)
    {
        return 0x2;
    }
    else if (strcmp(str, "Control") == 0)
    {
        return 0x3;
    }
    else if (strcmp(str, "HtmlDocument") == 0) // Note: Uncertain about this one
    {
        return 0x4;
    }
    else if (strcmp(str, "LegalInformation") == 0)
    {
        return 0x5;
    }
    else if (strcmp(str, "DeltaFragment") == 0)
    {
        return 0x6;
    }

    return 0x0;
}

static Result parseContent(NcmContentRecord *contentRecordsOut, struct xml_document *document)
{
    Result rc = 0;
    struct xml_node *root = xml_document_root(document);
    size_t numRootChildren = xml_node_children(root);
    size_t contentIndex = 1;

    for (int i = 0; i < numRootChildren; i++)
    {
        struct xml_node *child = xml_node_child(root, i);
        char *nodeName = (char *)xml_easy_name(child);

        if (strcmp(nodeName, "Content") == 0)
        {
            size_t numContentChildNodes = xml_node_children(child);
        
            for (int j = 0; j < numContentChildNodes; j++)
            {
                struct xml_node *contentChild = xml_node_child(child, j);
                char *contentChildNodeName = (char *)xml_easy_name(contentChild);
            
                if (strcmp(contentChildNodeName, "Type") == 0)
                {
                    char *nodeContent = (char *)xml_easy_content(contentChild);
                    contentRecordsOut[contentIndex].type = getContentTypeCode(nodeContent);
                    free(nodeContent);
                }
                else if (strcmp(contentChildNodeName, "Id") == 0)
                {
                    char *nodeContent = (char *)xml_easy_content(contentChild);

                    if (strlen(nodeContent) != 32) // 32 chars = 16 bytes
                    {
                        printf("parseContent: Incorrect id length\n");
                        rc = -1;
                        free(nodeContent);
                        free(contentChildNodeName);
                        free(nodeName);
                        goto CLEANUP;
                    }

                    char lowerU64[18] = {0};
                    char upperU64[18] = {0};
                    memcpy(lowerU64, nodeContent, 16);
                    memcpy(upperU64, nodeContent + 16, 16);

                    *(u64 *)contentRecordsOut[contentIndex].ncaId.c = __bswap64(strtoul(lowerU64, NULL, 16));
                    *(u64 *)(contentRecordsOut[contentIndex].ncaId.c + 8) = __bswap64(strtoul(upperU64, NULL, 16));
                    free(nodeContent);
                }
                else if (strcmp(contentChildNodeName, "Size") == 0)
                {
                    char *nodeContent = (char *)xml_easy_content(contentChild);
                    u8 type = contentRecordsOut[contentIndex].type;
                    *(u64 *)contentRecordsOut[contentIndex].size = strtol(nodeContent, NULL, 10) & 0xFFFFFFFFFFFF;
                    contentRecordsOut[contentIndex].type = type;
                    free(nodeContent);
                }

                free(contentChildNodeName);
            }

            contentIndex++;
        }

        free(nodeName);
    }

    CLEANUP:
    return rc;
}

Result parseXML(NcmMetaRecord *metaRecordOut, NcmContentRecord **contentRecordsOut, size_t *numContentRecordsOut, u8 *xmlBuf, size_t xmlBufSize)
{
    Result rc = 0;
    struct xml_document *document = xml_parse_document(xmlBuf, xmlBufSize);

    if (!document)
    {
        printf("installTitle: Failed to parse XML document");
        rc = -1;
        goto CLEANUP;
    }

    struct xml_node *root = xml_document_root(document);
    size_t numRootChildren = xml_node_children(root);
    size_t numContentEntries = 1; // Master content record isn't a "Content" entry

    u64 patchId = 0;
    u32 requiredSystemVersion = 0;

    for (int i = 0; i < numRootChildren; i++)
    {
        struct xml_node *child = xml_node_child(root, i);
        char *nodeName = (char *)xml_easy_name(child);

        if (strcmp(nodeName, "Type") == 0)
        {
            char *nodeContent = (char *)xml_easy_content(child);
            metaRecordOut->type = getTitleTypeCode(nodeContent);
            free(nodeContent);

            if (metaRecordOut->type != 0x80)
            {
                printf("parseXML: Invalid content meta type %x\n", metaRecordOut->type);
                rc = -1;
                goto CLEANUP;
            }
        }
        else if (strcmp(nodeName, "Id") == 0)
        {
            char *nodeContent = (char *)xml_easy_content(child);

            if (strlen(nodeContent) != 18)
            {
                printf("parseXML: Incorrect id length\n");
                rc = -1;
                free(nodeContent);
                goto CLEANUP;
            }

            u64 titleId = strtol(strrchr(nodeContent, 'x') + 1, NULL, 16);
            metaRecordOut->titleId = titleId;
            free(nodeContent);
        }
        else if (strcmp(nodeName, "Version") == 0)
        {
            char *nodeContent = (char *)xml_easy_content(child);
            metaRecordOut->version = (u32)strtol(nodeContent, NULL, 10);
            free(nodeContent);
        }
        else if (strcmp(nodeName, "Content") == 0)
        {
            numContentEntries++;
        }
        else if (strcmp(nodeName, "RequiredSystemVersion") == 0)
        {
            char *nodeContent = (char *)xml_easy_content(child);
            requiredSystemVersion = (u32)strtol(nodeContent, NULL, 10);
            free(nodeContent);
        }
        else if (strcmp(nodeName, "PatchId") == 0)
        {
            char *nodeContent = (char *)xml_easy_content(child);

            if (strlen(nodeContent) != 18)
            {
                printf("parseXML: Incorrect id length\n");
                rc = -1;
                free(nodeContent);
                goto CLEANUP;
            }

            patchId = strtol(strrchr(nodeContent, 'x') + 1, NULL, 16);
            free(nodeContent);
        }

        free(nodeName);
    }

    NcmContentRecord *contentRecords = calloc(numContentEntries, sizeof(NcmContentRecord));

    *(u64 *)(contentRecords) = 0x1000000000040010; // Magic
    *(u64 *)(((u8 *)contentRecords) + 8) = patchId;
    *(u32 *)(((u8 *)contentRecords) + 16) = requiredSystemVersion;

    if (R_FAILED(rc = parseContent(contentRecords, document)))
    {
        printf("parseXML: Failed to parse content records. Error code: 0x%08x\n", rc);
        free(contentRecords);
        goto CLEANUP;
    }

    *contentRecordsOut = contentRecords;
    *numContentRecordsOut = numContentEntries;

    CLEANUP:
    xml_document_free(document, false);
    return rc;
}