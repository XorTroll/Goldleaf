using LibHac;
using Microsoft.AspNetCore.Mvc;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Goldtree
{
    [ApiController]
    [Route("/")]
    [Produces("application/goldleaf")]
    public class NSPController : ControllerBase
    {
        private readonly NSPRepository repository;

        public NSPController(NSPRepository repository)
        {
            this.repository = repository ?? throw new ArgumentNullException(nameof(repository));
        }

        [HttpGet("files")]
        public string[] GetFiles()
        {
            return repository.Files.Keys.ToArray();
        }

        [HttpGet("file/{filename}/info")]
        public NSPInfo[] GetNSPInfo([FromRoute] string filename)
        {
            Pfs pnsp = repository.Files[filename];
            if (pnsp == null)
                throw new FileNotFoundException("Could not find requested NSP", filename);

            NSPInfo[] result = new NSPInfo[pnsp.Files.Length];
            return pnsp.Files.Select(file =>
            {
                long offset = pnsp.HeaderSize + file.Offset;
                return new NSPInfo(file.Name, offset, file.Size);
            })
            .ToArray();
        }

        [HttpGet("file/{filename}/content/{id}")]
        public void GetNSPContent([FromRoute] string filename, int id)
        {
            repository.ReadFilePart(filename, id, HttpContext.Response.Body);
        }

        [HttpGet("file/{filename}/ticket")]
        public void GetNSPTicket([FromRoute] string filename)
        {
            repository.ReadFilePart(filename,
                repository.Files[filename].Files
                    .Where(e => e.Name.ToLower().EndsWith(".tik"))
                    .Select((_, index) => index)
                    .First()
                , HttpContext.Response.Body);
        }

        [HttpGet("file/{filename}/certificate")]
        public void GetNSPCertificate([FromRoute] string filename)
        {
            repository.ReadFilePart(filename,
                repository.Files[filename].Files
                    .Where(e => e.Name.ToLower().EndsWith(".cert"))
                    .Select((_, index) => index)
                    .First()
                , HttpContext.Response.Body);
        }

        [HttpPost("echo")]
        public void Echo()
        {
            using (BinaryWriter writer = new BinaryWriter(HttpContext.Response.Body))
            using (BinaryReader reader = new BinaryReader(HttpContext.Request.Body))
            {
                uint size = (uint)HttpContext.Request.Body.Length;
                writer.Write(size);
                byte[] buffer = new byte[0x1000];

                while(size > 0)
                {
                    int read = reader.Read(buffer, 0, buffer.Length);
                    writer.Write(buffer, 0, read);
                    size -= (uint)read;
                }
            }
        }
    }
}
