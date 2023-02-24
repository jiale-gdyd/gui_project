#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rtsp/internal/util.h>
#include <rtsp/internal/our_md5.h>
#include <rtsp/internal/DigestAuthentication.hh>

namespace rtsp {
Authenticator::Authenticator()
{
    assign(NULL, NULL, NULL, NULL, false);
}

Authenticator::Authenticator(const Authenticator &orig)
{
    assign(orig.realm(), orig.nonce(), orig.username(), orig.password(), orig.fPasswordIsMD5);
}

Authenticator &Authenticator::operator=(const Authenticator &rightSide)
{
    if (&rightSide != this) {
        reset();
        assign(rightSide.realm(), rightSide.nonce(),
        rightSide.username(), rightSide.password(), rightSide.fPasswordIsMD5);
    }

    return *this;
}

Authenticator::~Authenticator()
{
    reset();
}

void Authenticator::reset()
{
    resetRealmAndNonce();
    resetUsernameAndPassword();
}

void Authenticator::setRealmAndNonce(char const *realm, char const *nonce)
{
    resetRealmAndNonce();
    assignRealmAndNonce(realm, nonce);
}

void Authenticator::setRealmAndRandomNonce(char const *realm)
{
    resetRealmAndNonce();

    struct {
        struct timeval timestamp;
        unsigned       counter;
    } seedData;

    gettimeofday(&seedData.timestamp, NULL);
    static unsigned counter = 0;
    seedData.counter = ++counter;

    char nonceBuf[33];
    our_MD5Data((unsigned char *)(&seedData), sizeof(seedData), nonceBuf);

    assignRealmAndNonce(realm, nonceBuf);
}

void Authenticator::setUsernameAndPassword(char const *username, char const *password, bool passwordIsMD5)
{
    resetUsernameAndPassword();
    assignUsernameAndPassword(username, password, passwordIsMD5);
}

char const *Authenticator::computeDigestResponse(char const *cmd, char const *url) const
{
    char ha1Buf[33];

    if (fPasswordIsMD5) {
        strncpy(ha1Buf, password(), 32);
        ha1Buf[32] = '\0';
    } else {
        unsigned const ha1DataLen = strlen(username()) + 1 + strlen(realm()) + 1 + strlen(password());
        unsigned char *ha1Data = new unsigned char[ha1DataLen + 1];

        sprintf((char *)ha1Data, "%s:%s:%s", username(), realm(), password());
        our_MD5Data(ha1Data, ha1DataLen, ha1Buf);
        delete[] ha1Data;
    }

    unsigned const ha2DataLen = strlen(cmd) + 1 + strlen(url);
    unsigned char *ha2Data = new unsigned char[ha2DataLen + 1];

    sprintf((char *)ha2Data, "%s:%s", cmd, url);

    char ha2Buf[33];
    our_MD5Data(ha2Data, ha2DataLen, ha2Buf);
    delete[] ha2Data;

    unsigned const digestDataLen = 32 + 1 + strlen(nonce()) + 1 + 32;
    unsigned char *digestData = new unsigned char[digestDataLen + 1];

    sprintf((char *)digestData, "%s:%s:%s", ha1Buf, nonce(), ha2Buf);
    char const *result = our_MD5Data(digestData, digestDataLen, NULL);

    delete[] digestData;
    return result;
}

void Authenticator::reclaimDigestResponse(char const *responseStr) const
{
    free((char *)responseStr);
}

void Authenticator::resetRealmAndNonce()
{
    delete[] fRealm;
    fRealm = NULL;
    delete[] fNonce;
    fNonce = NULL;
}

void Authenticator::resetUsernameAndPassword()
{
    delete[] fUsername;
    fUsername = NULL;
    delete[] fPassword;
    fPassword = NULL;
    fPasswordIsMD5 = false;
}

void Authenticator::assignRealmAndNonce(char const *realm, char const *nonce)
{
    fRealm = strDup(realm);
    fNonce = strDup(nonce);
}

void Authenticator::assignUsernameAndPassword(char const *username, char const *password, bool passwordIsMD5)
{
    fUsername = strDup(username);
    fPassword = strDup(password);
    fPasswordIsMD5 = passwordIsMD5;
}

void Authenticator::assign(char const *realm, char const *nonce, char const *username, char const *password, bool passwordIsMD5)
{
    assignRealmAndNonce(realm, nonce);
    assignUsernameAndPassword(username, password, passwordIsMD5);
}
}
