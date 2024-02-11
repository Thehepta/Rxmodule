

#include <android/log.h>
#include <string>
#include <jni.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <bits/ioctl.h>
#include <fstream>
#include <unistd.h>
#include <sys/socket.h>
#include <net/ethernet.h>
#include <netpacket/packet.h>
#include <linux/rtnetlink.h>
#include <dlfcn.h>
//#include "hookvpn/hookvpn.h"
#include "solist/solist.h"
#define LOG_TAG "checkVpn Native"

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

using namespace std;


//struct sockaddr_ll
//
//{
//
//    unsigned short int sll_family; /* 一般为AF_PACKET */
//    unsigned short int sll_protocol; /* 上层协议 */
//    int sll_ifindex; /* 接口类型 */
//    unsigned short int sll_hatype; /* 报头类型 */
//    unsigned char sll_pkttype; /* 包类型 */
//    unsigned char sll_halen; /* 地址长度 */
//    unsigned char sll_addr[8]; /* MAC地址 */
//
//};

int (*getifaddrs_org)(struct ifaddrs** __list_ptr) ;

int getifaddrs_hook(struct ifaddrs** __list_ptr){

    int re = getifaddrs_org(__list_ptr);

    LOGE("getifaddrs_hook ");
    struct ifaddrs *ifc = reinterpret_cast<ifaddrs *>(*__list_ptr);
    char ip[64] = {};
    char nm[64] = {};
    jint ifCount = 0;
    for (ifaddrs* ifa = ifc; ifa != NULL; ifa = ifa->ifa_next) {
        ++ifCount;
    }

    int index = 0;
    for (ifaddrs* ifa = ifc; ifa != NULL; ifa = ifa->ifa_next, ++index) {
        LOGE("%s", ifa->ifa_name);

    }
    return re ;
}

//mac地址好像有点问题
extern "C"
JNIEXPORT void JNICALL
Java_com_hepta_checkvpn_MainActivity_getifaddrs(JNIEnv *env, jobject thiz) {
    // TODO: implement checkvpn()


    struct ifaddrs *ifc, *ifc1;
    char ip[64] = {};
    char nm[64] = {};

    if(0 != getifaddrs(&ifc)) return ;
    ifc1 = ifc;

    LOGE("iface\tIP address\tNetmask\n");
    for(; NULL != ifc; ifc = (*ifc).ifa_next){
        LOGE("%s", (*ifc).ifa_name);
        if(NULL != (*ifc).ifa_addr) {
            inet_ntop(AF_INET, &(((struct sockaddr_in*)((*ifc).ifa_addr))->sin_addr), ip, 64);
            LOGE("\t%s", ip);
        }else{
            LOGE("\t\t");
        }
        if(NULL != (*ifc).ifa_netmask){
            inet_ntop(AF_INET, &(((struct sockaddr_in*)((*ifc).ifa_netmask))->sin_addr), nm, 64);
            LOGE("\t%s", nm);
        }else{
            LOGE("\t\t");
        }
        if(NULL != (*ifc).ifa_netmask){
            inet_ntop(AF_INET, &(((struct sockaddr_in*)((*ifc).ifa_netmask))->sin_addr), nm, 64);
            LOGE("\t%s", nm);
        }else{
            LOGE("\t\t");
        }
        struct sockaddr_ll *ll_addr = (struct sockaddr_ll *)ifc->ifa_addr;

//        LOGE("MAC address: %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x\n",  //这个可能是是ipv6地址
//             ll_addr->sll_addr[4], ll_addr->sll_addr[5], ll_addr->sll_addr[6], ll_addr->sll_addr[7], ll_addr->sll_addr[8], ll_addr->sll_addr[9],ll_addr->sll_addr[10],ll_addr->sll_addr[11],ll_addr->sll_addr[12],ll_addr->sll_addr[13]);

        LOGE("\n");
    }
    //freeifaddrs(ifc);
    freeifaddrs(ifc1);


}











//无效
extern "C"
JNIEXPORT void JNICALL
Java_com_hepta_checkvpn_MainActivity_getMac(JNIEnv *env, jobject thiz) {
    int sock_mac;

    struct ifreq ifr_mac;
    char mac_addr[30];

    sock_mac = socket( AF_INET, SOCK_STREAM, 0 );
    if( sock_mac == -1)
    {
        LOGE("create socket falise...mac/n");
        return ;
    }

    memset(&ifr_mac,0,sizeof(ifr_mac));
    strncpy(ifr_mac.ifr_name, "wlan0", sizeof(ifr_mac.ifr_name)-1);

    if( (ioctl( sock_mac, SIOCGIFHWADDR, &ifr_mac)) < 0)
    {
        LOGE("mac ioctl error/n");  //这里报错
        return ;
    }

    sprintf(mac_addr,"%02x%02x%02x%02x%02x%02x",
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[0],
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[1],
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[2],
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[3],
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[4],
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[5]);

    LOGE("local mac:%s /n",mac_addr);

    close( sock_mac );
}



//失败
int get_mac_address(char *if_name, unsigned char *mac_addr) {
    if (!if_name || !mac_addr) {
        return -1;
    }
    char path[64];
    snprintf(path, sizeof(path), "/sys/class/net/%s/address", if_name);   //没有权限读文件
    FILE *fp = fopen(path, "r");
    if (!fp) {
        return -1;
    }
    int ret = fscanf(fp, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
                     &mac_addr[0], &mac_addr[1], &mac_addr[2],
                     &mac_addr[3], &mac_addr[4], &mac_addr[5]);
    fclose(fp);
    return (ret == 6) ? 0 : -1;
}



extern "C"
JNIEXPORT void JNICALL
Java_com_hepta_checkvpn_MainActivity_getMac2(JNIEnv *env, jobject thiz) {
    unsigned char mac_addr[6] = {0};
    if (get_mac_address("wlan0", mac_addr) < 0) {
        LOGE("Failed to get MAC address\n");
        return ;
    }

    LOGE("MAC address: %02x:%02x:%02x:%02x:%02x:%02x\n",
         mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
}





struct ifaddrs_storage {
    // Must come first, so that `ifaddrs_storage` is-a `ifaddrs`.
    ifaddrs ifa;

    // The interface index, so we can match RTM_NEWADDR messages with
    // earlier RTM_NEWLINK messages (to copy the interface flags).
    int interface_index;

    // Storage for the pointers in `ifa`.
    sockaddr_storage addr;
    sockaddr_storage netmask;
    sockaddr_storage ifa_ifu;
    char name[IFNAMSIZ + 1];

    explicit ifaddrs_storage(ifaddrs **list) {
        memset(this, 0, sizeof(*this));

        // push_front onto `list`.
        ifa.ifa_next = *list;
        *list = reinterpret_cast<ifaddrs *>(this);
    }
    void SetBroadcastAddress(int family, const void* data, size_t byteCount) {

        ifa.ifa_broadaddr = CopyAddress(family, data, byteCount, &ifa_ifu);
    }

    void SetAddress(int family, const void* data, size_t byteCount) {
        // The kernel currently uses the order IFA_ADDRESS, IFA_LOCAL, IFA_BROADCAST
        // in inet_fill_ifaddr, but let's not assume that will always be true...
        if (ifa.ifa_addr == nullptr) {
            // This is an IFA_ADDRESS and haven't seen an IFA_LOCAL yet, so assume this is the
            // local address. SetLocalAddress will fix things if we later see an IFA_LOCAL.
            ifa.ifa_addr = CopyAddress(family, data, byteCount, &addr);
        } else {
            // We already saw an IFA_LOCAL, which implies this is a destination address.
            ifa.ifa_dstaddr = CopyAddress(family, data, byteCount, &ifa_ifu);
        }
    }

    void SetPacketAttributes(int ifindex, unsigned short hatype, unsigned char halen) {
        sockaddr_ll* sll = reinterpret_cast<sockaddr_ll*>(&addr);
        sll->sll_ifindex = ifindex;
        sll->sll_hatype = hatype;
        sll->sll_halen = halen;
    }

    void SetLocalAddress(int family, const void* data, size_t byteCount) {
        // The kernel source says "for point-to-point IFA_ADDRESS is DESTINATION address,
        // local address is supplied in IFA_LOCAL attribute".
        //   -- http://lxr.free-electrons.com/source/include/uapi/linux/if_addr.h#L17

        // So copy any existing IFA_ADDRESS into ifa_dstaddr...
        if (ifa.ifa_addr != nullptr) {
            ifa.ifa_dstaddr = reinterpret_cast<sockaddr*>(memcpy(&ifa_ifu, &addr, sizeof(addr)));
        }
        // ...and then put this IFA_LOCAL into ifa_addr.
        ifa.ifa_addr = CopyAddress(family, data, byteCount, &addr);
    }


    sockaddr* CopyAddress(int family, const void* data, size_t byteCount, sockaddr_storage* ss) {
        // Netlink gives us the address family in the header, and the
        // sockaddr_in or sockaddr_in6 bytes as the payload. We need to
        // stitch the two bits together into the sockaddr that's part of
        // our portable interface.
        ss->ss_family = family;
        memcpy(SockaddrBytes(family, ss), data, byteCount);

        // For IPv6 we might also have to set the scope id.
        if (family == AF_INET6 && (IN6_IS_ADDR_LINKLOCAL(data) || IN6_IS_ADDR_MC_LINKLOCAL(data))) {
            reinterpret_cast<sockaddr_in6*>(ss)->sin6_scope_id = interface_index;
        }

        return reinterpret_cast<sockaddr*>(ss);
    }


    uint8_t* SockaddrBytes(int family, sockaddr_storage* ss) {
        if (family == AF_INET) {
            sockaddr_in* ss4 = reinterpret_cast<sockaddr_in*>(ss);
            return reinterpret_cast<uint8_t*>(&ss4->sin_addr);
        } else if (family == AF_INET6) {
            sockaddr_in6* ss6 = reinterpret_cast<sockaddr_in6*>(ss);
            return reinterpret_cast<uint8_t*>(&ss6->sin6_addr);
        } else if (family == AF_PACKET) {
            sockaddr_ll* sll = reinterpret_cast<sockaddr_ll*>(ss);
            return reinterpret_cast<uint8_t*>(&sll->sll_addr);
        }
        return nullptr;
    }

    void SetNetmask(int family, size_t prefix_length) {
        // ...and work out the netmask from the prefix length.
        netmask.ss_family = family;
        uint8_t* dst = SockaddrBytes(family, &netmask);
        memset(dst, 0xff, prefix_length / 8);
        if ((prefix_length % 8) != 0) {
            dst[prefix_length/8] = (0xff << (8 - (prefix_length % 8)));
        }
        ifa.ifa_netmask = reinterpret_cast<sockaddr*>(&netmask);
    }

};





#define TEMP_FAILURE_RETRY(exp) ({         \
      __typeof__(exp) _rc;                   \
      do {                                   \
          _rc = (exp);                       \
      } while (_rc == -1 && errno == EINTR); \
      _rc; })


static void __getifaddrs_callback(void* context, nlmsghdr* hdr) {
    ifaddrs** out = reinterpret_cast<ifaddrs**>(context);

    if (hdr->nlmsg_type == RTM_NEWLINK) {
        LOGE("__getifaddrs_callback RTM_NEWLINK");
        ifinfomsg* ifi = reinterpret_cast<ifinfomsg*>(NLMSG_DATA(hdr));

        // Create a new ifaddr entry, and set the interface index and flags.
        ifaddrs_storage* new_addr = new ifaddrs_storage(out);
        new_addr->interface_index = ifi->ifi_index;
        new_addr->ifa.ifa_flags = ifi->ifi_flags;

        // Go through the various bits of information and find the name.
        rtattr* rta = IFLA_RTA(ifi);
        size_t rta_len = IFLA_PAYLOAD(hdr);
        while (RTA_OK(rta, rta_len)) {
            if (rta->rta_type == IFLA_ADDRESS) {
                if (RTA_PAYLOAD(rta) < sizeof(new_addr->addr)) {
                    new_addr->SetAddress(AF_PACKET, RTA_DATA(rta), RTA_PAYLOAD(rta));
                    new_addr->SetPacketAttributes(ifi->ifi_index, ifi->ifi_type, RTA_PAYLOAD(rta));
                }
            } else if (rta->rta_type == IFLA_BROADCAST) {
                if (RTA_PAYLOAD(rta) < sizeof(new_addr->ifa_ifu)) {
                    new_addr->SetBroadcastAddress(AF_PACKET, RTA_DATA(rta), RTA_PAYLOAD(rta));
                    new_addr->SetPacketAttributes(ifi->ifi_index, ifi->ifi_type, RTA_PAYLOAD(rta));
                }
            } else if (rta->rta_type == IFLA_IFNAME) {
                if (RTA_PAYLOAD(rta) < sizeof(new_addr->name)) {
                    memcpy(new_addr->name, RTA_DATA(rta), RTA_PAYLOAD(rta));
                    new_addr->ifa.ifa_name = new_addr->name;
                }
            }
            rta = RTA_NEXT(rta, rta_len);
        }
    }else if (hdr->nlmsg_type == RTM_NEWADDR) {
        LOGE("__getifaddrs_callback RTM_NEWADDR");

        ifaddrmsg* msg = reinterpret_cast<ifaddrmsg*>(NLMSG_DATA(hdr));
        const ifaddrs_storage* known_addr = reinterpret_cast<const ifaddrs_storage*>(*out);
//        const ifaddrs_storage* known_addr;
        while (known_addr != nullptr && known_addr->interface_index != static_cast<int>(msg->ifa_index)) {
            known_addr = reinterpret_cast<ifaddrs_storage*>(known_addr->ifa.ifa_next);
        }
        ifaddrs_storage* new_addr = new ifaddrs_storage(out);
        new_addr->interface_index = static_cast<int>(msg->ifa_index);

        if (known_addr != nullptr) {
            strcpy(new_addr->name, known_addr->name);
            new_addr->ifa.ifa_name = new_addr->name;
            new_addr->ifa.ifa_flags = known_addr->ifa.ifa_flags;

        }
        rtattr* rta = IFA_RTA(msg);
        size_t rta_len = IFA_PAYLOAD(hdr);
        while (RTA_OK(rta, rta_len)) {
            if (rta->rta_type == IFA_ADDRESS) {
                if (msg->ifa_family == AF_INET || msg->ifa_family == AF_INET6) {
                    new_addr->SetAddress(msg->ifa_family, RTA_DATA(rta), RTA_PAYLOAD(rta));
                    new_addr->SetNetmask(msg->ifa_family, msg->ifa_prefixlen);
                }
            } else if (rta->rta_type == IFA_BROADCAST) {
                if (msg->ifa_family == AF_INET) {
                    new_addr->SetBroadcastAddress(msg->ifa_family, RTA_DATA(rta), RTA_PAYLOAD(rta));
                    if (known_addr == nullptr) {
                        // We did not read the broadcast flag from an RTM_NEWLINK message.
                        // Ensure that it is set.
                        new_addr->ifa.ifa_flags |= IFF_BROADCAST;
                    }
                }
            } else if (rta->rta_type == IFA_LOCAL) {
                if (msg->ifa_family == AF_INET || msg->ifa_family == AF_INET6) {
                    new_addr->SetLocalAddress(msg->ifa_family, RTA_DATA(rta), RTA_PAYLOAD(rta));
                }
            } else if (rta->rta_type == IFA_LABEL) {
                if (RTA_PAYLOAD(rta) < sizeof(new_addr->name)) {
                    memcpy(new_addr->name, RTA_DATA(rta), RTA_PAYLOAD(rta));
                    new_addr->ifa.ifa_name = new_addr->name;
                }
            }
            rta = RTA_NEXT(rta, rta_len);
        }
    }
}


size_t size_ = 8192;
char data_[8192];
int sockfd;


bool SendRequest(int type) {
    // Rather than force all callers to check for the unlikely event of being
    // unable to allocate 8KiB, check here.


    sockfd =socket(PF_NETLINK, SOCK_RAW | SOCK_CLOEXEC, NETLINK_ROUTE);


    // Construct and send the message.
    struct NetlinkMessage {
        nlmsghdr hdr;
        rtgenmsg msg;
    } request;
    memset(&request, 0, sizeof(request));
    request.hdr.nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST;
    request.hdr.nlmsg_type = type;
    request.hdr.nlmsg_len = sizeof(request);
    request.msg.rtgen_family = AF_UNSPEC; // All families.
    return (TEMP_FAILURE_RETRY(send(sockfd, &request, sizeof(request), 0)) == sizeof(request));
}


bool ReadResponses(void callback(void*, nlmsghdr*), void* context) {
    // Read through all the responses, handing interesting ones to the callback.
    ssize_t bytes_read;
    while ((bytes_read = TEMP_FAILURE_RETRY(recv(sockfd, data_, size_, 0))) > 0) {
        nlmsghdr* hdr = reinterpret_cast<nlmsghdr*>(data_);
        for (; NLMSG_OK(hdr, static_cast<size_t>(bytes_read)); hdr = NLMSG_NEXT(hdr, bytes_read)) {
            if (hdr->nlmsg_type == NLMSG_DONE) return true;
            if (hdr->nlmsg_type == NLMSG_ERROR) {
                nlmsgerr* err = reinterpret_cast<nlmsgerr*>(NLMSG_DATA(hdr));
                errno = (hdr->nlmsg_len >= NLMSG_LENGTH(sizeof(nlmsgerr))) ? -err->error : EIO;
                return false;
            }
            callback(context, hdr);
        }
    }

    // We only get here if recv fails before we see a NLMSG_DONE.
    return false;
}


void *symlistFun(void *arg){
    text();




    return nullptr;
}



extern "C"
JNIEXPORT void JNICALL
Java_com_hepta_checkvpn_MainActivity_net_1us(JNIEnv *env, jobject thiz) {

    struct ifaddrs *ifc, *out;
    char ip[64] = {};
    char nm[64] = {};
    // Open the netlink socket and ask for all the links and addresses.
    // SELinux policy only allows RTM_GETLINK messages to be sent by system apps.
    out = nullptr;
    bool getaddr_success =    SendRequest(RTM_GETADDR) && ReadResponses(__getifaddrs_callback, &out);

    if (!getaddr_success) {
        freeifaddrs(out);
        // Ensure that callers crash if they forget to check for success.
        out = nullptr;
        LOGE("FAILED");
    } else{
        LOGE("getaddr_success");

    }

    ifc = out;

    LOGE("iface\tIP address\tNetmask\n");
    for(; NULL != ifc; ifc = (*ifc).ifa_next){
        LOGE("%s", (*ifc).ifa_name);
        if(NULL != (*ifc).ifa_addr) {
            inet_ntop(AF_INET, &(((struct sockaddr_in*)((*ifc).ifa_addr))->sin_addr), ip, 64);
            LOGE("\t%s", ip);
        }else{
            LOGE("\t\t");
        }
        if(NULL != (*ifc).ifa_netmask){
            inet_ntop(AF_INET, &(((struct sockaddr_in*)((*ifc).ifa_netmask))->sin_addr), nm, 64);
            LOGE("\t%s", nm);
        }else{
            LOGE("\t\t");
        }
        if(NULL != (*ifc).ifa_netmask){
            inet_ntop(AF_INET, &(((struct sockaddr_in*)((*ifc).ifa_netmask))->sin_addr), nm, 64);
            LOGE("\t%s", nm);
        }else{
            LOGE("\t\t");
        }
        struct sockaddr_ll *ll_addr = (struct sockaddr_ll *)ifc->ifa_addr;

//        LOGE("MAC address: %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x\n",  //这个可能是是ipv6地址
//             ll_addr->sll_addr[4], ll_addr->sll_addr[5], ll_addr->sll_addr[6], ll_addr->sll_addr[7], ll_addr->sll_addr[8], ll_addr->sll_addr[9],ll_addr->sll_addr[10],ll_addr->sll_addr[11],ll_addr->sll_addr[12],ll_addr->sll_addr[13]);

        LOGE("\n");
    }

//    hook_start();


    pthread_t tid;
    pthread_create(&tid, nullptr,symlistFun, nullptr);


}