using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using LibHac;
using LibHac.IO;
using Microsoft.Extensions.Configuration;

namespace Goldtree
{
    public class NSPRepository
    {
        public Dictionary<string, Pfs> Files { get; } = new Dictionary<string, Pfs>();

        public NSPRepository(IConfiguration configuration)
        {
            string filename = configuration["file"];
            if (!string.IsNullOrWhiteSpace(filename))
            {
                Files.Add(GetFilename(filename), LoadFile(filename));
            }

            string folder = configuration["folder"];
            if (!string.IsNullOrWhiteSpace(folder))
            {
                foreach (string file in SearchFolder(folder))
                {
                    Files.Add(GetFilename(file), LoadFile(file));
                }
            }

            if (string.IsNullOrWhiteSpace(filename) && string.IsNullOrWhiteSpace(folder))
            {
                foreach (string file in SearchFolder(Directory.GetCurrentDirectory()))
                {
                    Files.Add(GetFilename(file), LoadFile(file));
                }
            }
        }

        public void ReadFilePart(string filename, int id, Stream output)
        {
            Pfs pnsp = Files[filename];
            if (pnsp == null)
                throw new FileNotFoundException("Could not find requested NSP", filename);

            if (id < 0 || id >= pnsp.Files.Length)
                throw new FileNotFoundException("Could not find requested NSP Content", id.ToString());

            PfsFileEntry file = pnsp.Files[id];

            using (BinaryReader reader = new BinaryReader(new FileStream(filename, FileMode.Open)))
            {
                reader.BaseStream.Seek(pnsp.HeaderSize + file.Offset, SeekOrigin.Begin);
                int size = (int)file.Size;
                int offset = 0;
                byte[] buffer = new byte[0x1000];
                while (size > 0)
                {
                    int read = reader.Read(buffer, offset, Math.Min(size, 0x1000));
                    size -= read;
                    offset += read;

                    output.Write(buffer, 0, read);
                }
            }
        }

        private string[] SearchFolder(string folder) => Directory.GetFiles(folder, "*.nsp", SearchOption.AllDirectories);

        private Pfs LoadFile(string file)
        {
            FileStream fs = new FileStream(file, FileMode.Open);
            StreamStorage ist = new StreamStorage(fs, true);
            return new Pfs(ist);
        }

        private string GetFilename(string file)
        {
            return new FileInfo(file).Name;
        }
    }
}
