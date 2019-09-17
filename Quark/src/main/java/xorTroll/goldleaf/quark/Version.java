package xorTroll.goldleaf.quark;

public class Version
{
    public byte major;
    public byte minor;
    public byte micro;

    public Version(int raw)
    {
        this.major = (byte)(((raw) >> 16) & 0xFF);
        this.minor = (byte)(((raw) >> 8) & 0xFF);
        this.micro = (byte)(raw & 0xFF);
    }

    public Version(int major, int minor, int micro)
    {
        this.major = (byte)major;
        this.minor = (byte)minor;
        this.micro = (byte)micro;
    }

    public boolean newerThan(Version v)
    {
        if(this.major > v.major) return true;
        if(this.major == v.major)
        {
            if(this.minor > v.minor) return true;
            if(this.minor == v.minor)
            {
                if(this.micro > v.micro) return true;
            }
        }
        return false;
    }

    public boolean olderThan(Version v)
    {
        return !same(v) && !newerThan(v);
    }

    public boolean same(Version v)
    {
        return (this.major == v.major) && (this.minor == v.minor) && (this.micro == v.micro);
    }

    @Override
    public String toString()
    {
        return this.major + "." + this.minor + "." + this.micro;
    }
}