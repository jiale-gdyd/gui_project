#ifndef RTSP_INTERNAL_DIGEST_AUTHENTICATION_HH
#define RTSP_INTERNAL_DIGEST_AUTHENTICATION_HH

namespace rtsp {
class Authenticator {
public:
    Authenticator();
    Authenticator(const Authenticator &orig);
    Authenticator &operator=(const Authenticator &rightSide);
    virtual ~Authenticator();

    void reset();
    void setRealmAndNonce(char const *realm, char const *nonce);
    void setRealmAndRandomNonce(char const *realm);

    void setUsernameAndPassword(char const *username, char const *password, bool passwordIsMD5 = false);

    char const *realm() const {
        return fRealm;
    }

    char const *nonce() const {
        return fNonce;
    }

    char const *username() const {
        return fUsername;
    }

    char const *password() const {
        return fPassword;
    }

    char const *computeDigestResponse(char const *cmd, char const *url) const;
    void reclaimDigestResponse(char const *responseStr) const;

private:
    void resetRealmAndNonce();
    void resetUsernameAndPassword();
    void assignRealmAndNonce(char const *realm, char const *nonce);
    void assignUsernameAndPassword(char const *username, char const *password, bool passwordIsMD5);
    void assign(char const *realm, char const *nonce, char const *username, char const *password, bool passwordIsMD5);

private:
    char *fRealm;
    char *fNonce;
    char *fUsername;
    char *fPassword;
    bool fPasswordIsMD5;
};
}

#endif
