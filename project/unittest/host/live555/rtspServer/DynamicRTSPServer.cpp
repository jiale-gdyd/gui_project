#include "DynamicRTSPServer.h"

#include <string.h>
#include <live555/liveMedia/liveMedia.hh>

DynamicRTSPServer *DynamicRTSPServer::createNew(UsageEnvironment &env, Port ourPort, UserAuthenticationDatabase *authDatabase, unsigned reclamationTestSeconds)
{
    int ourSocketIPv4 = setUpOurSocket(env, ourPort, AF_INET);
    int ourSocketIPv6 = setUpOurSocket(env, ourPort, AF_INET6);
    if ((ourSocketIPv4 < 0) && (ourSocketIPv6 < 0)) {
        return NULL;
    }

    return new DynamicRTSPServer(env, ourSocketIPv4, ourSocketIPv6, ourPort, authDatabase, reclamationTestSeconds);
}

DynamicRTSPServer::DynamicRTSPServer(UsageEnvironment &env, int ourSocketIPv4, int ourSocketIPv6, Port ourPort, UserAuthenticationDatabase *authDatabase, unsigned reclamationTestSeconds)
    : RTSPServer(env, ourSocketIPv4, ourSocketIPv6, ourPort, authDatabase, reclamationTestSeconds)
{

}

DynamicRTSPServer::~DynamicRTSPServer()
{

}

static ServerMediaSession* createNewSMS(UsageEnvironment &env, char const* fileName, FILE *fid);

void DynamicRTSPServer::lookupServerMediaSession(char const *streamName, lookupServerMediaSessionCompletionFunc *completionFunc, void *completionClientData, Boolean isFirstLookupInSession)
{
    FILE *fid = fopen(streamName, "rb");
    Boolean const fileExists = fid != NULL;

    ServerMediaSession *sms = getServerMediaSession(streamName);
    Boolean const smsExists = sms != NULL;

    if (!fileExists) {
        if (smsExists) {
            removeServerMediaSession(sms);
        }

        sms = NULL;
    } else {
        if (smsExists && isFirstLookupInSession) {
            removeServerMediaSession(sms); 
            sms = NULL;
        } 

        if (sms == NULL) {
            sms = createNewSMS(envir(), streamName, fid); 
            addServerMediaSession(sms);
        }

        fclose(fid);
    }

    if (completionFunc != NULL) {
        (*completionFunc)(completionClientData, sms);
    }
}

struct MatroskaDemuxCreationState {
    MatroskaFileServerDemux *demux;
    char                    watchVariable;
};

static void onMatroskaDemuxCreation(MatroskaFileServerDemux *newDemux, void *clientData)
{
    MatroskaDemuxCreationState *creationState = (MatroskaDemuxCreationState *)clientData;
    creationState->demux = newDemux;
    creationState->watchVariable = 1;
}

struct OggDemuxCreationState {
    OggFileServerDemux *demux;
    char               watchVariable;
};

static void onOggDemuxCreation(OggFileServerDemux *newDemux, void *clientData)
{
    OggDemuxCreationState *creationState = (OggDemuxCreationState *)clientData;
    creationState->demux = newDemux;
    creationState->watchVariable = 1;
}

#define NEW_SMS(description)                                                        \
    do {                                                                            \
        char const *descStr = description ", streamed by the LIVE555 Media Server"; \
        sms = ServerMediaSession::createNew(env, fileName, fileName, descStr);      \
    } while (0)

static ServerMediaSession *createNewSMS(UsageEnvironment &env, char const *fileName, FILE */*fid*/)
{
    char const *extension = strrchr(fileName, '.');
    if (extension == NULL) {
        return NULL;
    }

    ServerMediaSession *sms = NULL;
    Boolean const reuseSource = False;

    if (strcmp(extension, ".aac") == 0) {
        NEW_SMS("AAC Audio");
        sms->addSubsession(ADTSAudioFileServerMediaSubsession::createNew(env, fileName, reuseSource));
    } else if (strcmp(extension, ".amr") == 0) {
        NEW_SMS("AMR Audio");
        sms->addSubsession(AMRAudioFileServerMediaSubsession::createNew(env, fileName, reuseSource));
    } else if (strcmp(extension, ".ac3") == 0) {
        NEW_SMS("AC-3 Audio");
        sms->addSubsession(AC3AudioFileServerMediaSubsession::createNew(env, fileName, reuseSource));
    } else if (strcmp(extension, ".m4e") == 0) {
        NEW_SMS("MPEG-4 Video");
        sms->addSubsession(MPEG4VideoFileServerMediaSubsession::createNew(env, fileName, reuseSource));
    } else if (strcmp(extension, ".264") == 0) {
        NEW_SMS("H.264 Video");
        OutPacketBuffer::maxSize = 100000;
        sms->addSubsession(H264VideoFileServerMediaSubsession::createNew(env, fileName, reuseSource));
    } else if (strcmp(extension, ".265") == 0) {
        NEW_SMS("H.265 Video");
        OutPacketBuffer::maxSize = 100000;
        sms->addSubsession(H265VideoFileServerMediaSubsession::createNew(env, fileName, reuseSource));
    } else if (strcmp(extension, ".mp3") == 0) {
        NEW_SMS("MPEG-1 or 2 Audio");

        Boolean useADUs = False;
        Interleaving *interleaving = NULL;

#ifdef STREAM_USING_ADUS
        useADUs = True;
#ifdef INTERLEAVE_ADUS
        unsigned char interleaveCycle[] = {0, 2, 1, 3};
        unsigned const interleaveCycleSize = (sizeof interleaveCycle) / (sizeof (unsigned char));
        interleaving = new Interleaving(interleaveCycleSize, interleaveCycle);
#endif
#endif
        sms->addSubsession(MP3AudioFileServerMediaSubsession::createNew(env, fileName, reuseSource, useADUs, interleaving));
    } else if (strcmp(extension, ".mpg") == 0) {
        NEW_SMS("MPEG-1 or 2 Program Stream");
        MPEG1or2FileServerDemux *demux = MPEG1or2FileServerDemux::createNew(env, fileName, reuseSource);
        sms->addSubsession(demux->newVideoServerMediaSubsession());
        sms->addSubsession(demux->newAudioServerMediaSubsession());
    } else if (strcmp(extension, ".vob") == 0) {
        NEW_SMS("VOB (MPEG-2 video with AC-3 audio)");
        MPEG1or2FileServerDemux *demux = MPEG1or2FileServerDemux::createNew(env, fileName, reuseSource);
        sms->addSubsession(demux->newVideoServerMediaSubsession());
        sms->addSubsession(demux->newAC3AudioServerMediaSubsession());
    } else if (strcmp(extension, ".ts") == 0) {
        unsigned indexFileNameLen = strlen(fileName) + 2;
        char *indexFileName = new char[indexFileNameLen];
        sprintf(indexFileName, "%sx", fileName);
        NEW_SMS("MPEG Transport Stream");
        sms->addSubsession(MPEG2TransportFileServerMediaSubsession::createNew(env, fileName, indexFileName, reuseSource));
        delete[] indexFileName;
    } else if (strcmp(extension, ".wav") == 0) {
        NEW_SMS("WAV Audio Stream");
        Boolean convertToULaw = False;
        sms->addSubsession(WAVAudioFileServerMediaSubsession::createNew(env, fileName, reuseSource, convertToULaw));
    } else if (strcmp(extension, ".dv") == 0) {
        OutPacketBuffer::maxSize = 300000;

        NEW_SMS("DV Video");
        sms->addSubsession(DVVideoFileServerMediaSubsession::createNew(env, fileName, reuseSource));
    } else if (!strcmp(extension, ".mkv") || !strcmp(extension, ".webm")) {
        OutPacketBuffer::maxSize = 300000;
        NEW_SMS("Matroska video+audio+(optional)subtitles");

        MatroskaDemuxCreationState creationState;
        creationState.watchVariable = 0;
        MatroskaFileServerDemux::createNew(env, fileName, onMatroskaDemuxCreation, &creationState);
        env.taskScheduler().doEventLoop(&creationState.watchVariable);

        ServerMediaSubsession* smss;
        while ((smss = creationState.demux->newServerMediaSubsession()) != NULL) {
            sms->addSubsession(smss);
        }
    } else if (!strcmp(extension, ".ogg") || !strcmp(extension, ".ogv") || !strcmp(extension, ".opus")) {
        NEW_SMS("Ogg video and/or audio");

        OggDemuxCreationState creationState;
        creationState.watchVariable = 0;
        OggFileServerDemux::createNew(env, fileName, onOggDemuxCreation, &creationState);
        env.taskScheduler().doEventLoop(&creationState.watchVariable);

        ServerMediaSubsession *smss;
        while ((smss = creationState.demux->newServerMediaSubsession()) != NULL) {
            sms->addSubsession(smss);
        }
    }

    return sms;
}
