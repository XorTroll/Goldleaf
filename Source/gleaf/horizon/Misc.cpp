#include <gleaf/horizon/Misc.hpp>

namespace gleaf::horizon
{
    u32 GetBatteryLevel()
    {
        u32 bat = 0;
        Result rc = psmInitialize();
        if(R_FAILED(rc)) return bat;
        rc = psmGetBatteryChargePercentage(&bat);
        if(R_FAILED(rc)) bat = 0;
        psmExit();
        return bat;
    }

    bool IsCharging()
    {
        ChargerType charger = ChargerType_None;
        Result rc = psmInitialize();
        if(R_FAILED(rc)) return false;
        rc = psmGetChargerType(&charger);
        if(R_FAILED(rc)) charger = ChargerType_None;
        psmExit();
        return (charger > ChargerType_None);
    }

    std::string GetOpenedUserName()
    {
        std::string user = "<unknown>";
        u128 uid = 0;
        Result rc = accountInitialize();
        if(R_FAILED(rc)) return user;
        AccountProfile pr;
        AccountProfileBase bpr;
        AccountUserData udata;
        bool active = false;
        rc = accountGetActiveUser(&uid, &active);
        if(R_FAILED(rc))
        {
            accountExit();
            return user;
        }
        rc = accountGetProfile(&pr, uid);
        if(R_FAILED(rc))
        {
            accountExit();
            return user;
        }
        rc = accountProfileGet(&pr, &udata, &bpr);
        if(R_FAILED(rc))
        {
            accountExit();
            return user;
        }
        user = std::string(bpr.username);
        accountExit();
        return user;
    }

    bool IsUserSelected()
    {
        bool sel = false;
        u128 uid = 0;
        Result rc = accountInitialize();
        if(R_FAILED(rc)) return sel;
        rc = accountGetActiveUser(&uid, &sel);
        if(R_FAILED(rc)) sel = false;
        accountExit();
        return sel;
    }

    std::string GetCurrentTime()
    {
        time_t timet = time(NULL);
        struct tm *times = localtime((const time_t*)&timet);
        int h = times->tm_hour;
        int min = times->tm_min;
        int s = times->tm_sec;
        char timestr[9];
        sprintf(timestr, "%02d:%02d:%02d", h, min, s);
        return std::string(timestr);
    }
}