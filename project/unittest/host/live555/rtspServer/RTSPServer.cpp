#include <live555/groupsock/GroupsockHelper.hh>
#include <live555/BasicUsageEnvironment/BasicUsageEnvironment.hh>

#include "RTSPServer.h"
#include "DynamicRTSPServer.h"

int live555_rtspserver_main(int argc, char *argv[])
{
    TaskScheduler *scheduler = BasicTaskScheduler::createNew();
    UsageEnvironment *env = BasicUsageEnvironment::createNew(*scheduler);

    UserAuthenticationDatabase *authDB = NULL;
#ifdef ACCESS_CONTROL
    authDB = new UserAuthenticationDatabase;
    authDB->addUserRecord("username1", "password1");
#endif

    RTSPServer *rtspServer;
    portNumBits rtspServerPortNum = 554;

    rtspServer = DynamicRTSPServer::createNew(*env, rtspServerPortNum, authDB);
    if (rtspServer == NULL) {
        rtspServerPortNum = 8554;
        rtspServer = DynamicRTSPServer::createNew(*env, rtspServerPortNum, authDB);
    }

    if (rtspServer == NULL) {
        *env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
        exit(1);
    }

    *env << "LIVE555 Media Server\n";
    *env << "\tversion " << MEDIA_SERVER_VERSION_STRING << " (LIVE555 Streaming Media library version " << LIVEMEDIA_LIBRARY_VERSION_STRING << ").\n";

    *env << "Play streams from this server using the URL\n";
    if (weHaveAnIPv4Address(*env)) {
        char *rtspURLPrefix = rtspServer->ipv4rtspURLPrefix();
        *env << "\t" << rtspURLPrefix << "<filename>\n";
        delete[] rtspURLPrefix;
        if (weHaveAnIPv6Address(*env)) {
            *env << "or\n";
        }
    }

    if (weHaveAnIPv6Address(*env)) {
        char *rtspURLPrefix = rtspServer->ipv6rtspURLPrefix();
        *env << "\t" << rtspURLPrefix << "<filename>\n";
        delete[] rtspURLPrefix;
    }
    *env << "where <filename> is a file present in the current directory.\n";

    *env << "Each file's type is inferred from its name suffix:\n";
    *env << "\t\".264\" => a H.264 Video Elementary Stream file\n";
    *env << "\t\".265\" => a H.265 Video Elementary Stream file\n";
    *env << "\t\".aac\" => an AAC Audio (ADTS format) file\n";
    *env << "\t\".ac3\" => an AC-3 Audio file\n";
    *env << "\t\".amr\" => an AMR Audio file\n";
    *env << "\t\".dv\" => a DV Video file\n";
    *env << "\t\".m4e\" => a MPEG-4 Video Elementary Stream file\n";
    *env << "\t\".mkv\" => a Matroska audio+video+(optional)subtitles file\n";
    *env << "\t\".mp3\" => a MPEG-1 or 2 Audio file\n";
    *env << "\t\".mpg\" => a MPEG-1 or 2 Program Stream (audio+video) file\n";
    *env << "\t\".ogg\" or \".ogv\" or \".opus\" => an Ogg audio and/or video file\n";
    *env << "\t\".ts\" => a MPEG Transport Stream file\n";
    *env << "\t\t(a \".tsx\" index file - if present - provides server 'trick play' support)\n";
    *env << "\t\".vob\" => a VOB (MPEG-2 video with AC-3 audio) file\n";
    *env << "\t\".wav\" => a WAV Audio file\n";
    *env << "\t\".webm\" => a WebM audio(Vorbis)+video(VP8) file\n";
    *env << "See http://www.live555.com/mediaServer/ for additional documentation.\n";

    if (rtspServer->setUpTunnelingOverHTTP(80) || rtspServer->setUpTunnelingOverHTTP(8000) || rtspServer->setUpTunnelingOverHTTP(8080)) {
        *env << "(We use port " << rtspServer->httpServerPortNum() << " for optional RTSP-over-HTTP tunneling).)\n";
    } else {
        *env << "(RTSP-over-HTTP tunneling is not available.)\n";
    }

    env->taskScheduler().doEventLoop();
    return 0;
}
