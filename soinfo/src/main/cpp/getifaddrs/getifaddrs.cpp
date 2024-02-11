//
// Created by chic on 2023/5/30.
//



//
// Created by chic on 2023/5/30.
//

#include <cstdio>
#include <ifaddrs.h>
#include <arpa/inet.h>

int main(){

    struct ifaddrs *ifc, *ifc1;
    char ip[64] = {};
    char nm[64] = {};

    if(0 != getifaddrs(&ifc)) return 0 ;
    ifc1 = ifc;

    printf("iface\tIP address\tNetmask\n");
    for(; NULL != ifc; ifc = (*ifc).ifa_next){
        printf("%s", (*ifc).ifa_name);
        if(NULL != (*ifc).ifa_addr) {
            inet_ntop(AF_INET, &(((struct sockaddr_in*)((*ifc).ifa_addr))->sin_addr), ip, 64);
            printf("\t%s", ip);
        }else{
            printf("\t\t");
        }
        if(NULL != (*ifc).ifa_netmask){
            inet_ntop(AF_INET, &(((struct sockaddr_in*)((*ifc).ifa_netmask))->sin_addr), nm, 64);
            printf("\t%s", nm);
        }else{
            printf("\t\t");
        }
        if(NULL != (*ifc).ifa_netmask){
            inet_ntop(AF_INET, &(((struct sockaddr_in*)((*ifc).ifa_netmask))->sin_addr), nm, 64);
            printf("\t%s", nm);
        }else{
            printf("\t\t");
        }
        struct sockaddr_ll *ll_addr = (struct sockaddr_ll *)ifc->ifa_addr;


        printf("\n");
    }
    freeifaddrs(ifc1);
    return 0;
}