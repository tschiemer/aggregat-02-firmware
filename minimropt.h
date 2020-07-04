//
// Created by Philip Tschiemer on 26.06.20.
//

#ifndef MINIMR_MINIMROPT_H
#define MINIMR_MINIMROPT_H

// #include "config.h"
// #include <cstdint>
typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef unsigned short uint16_t;
typedef signed short int16_t;
typedef unsigned int uint32_t;
typedef signed int int32_t;


#define MINIMR_TIMESTAMP_USE 1
#define MINIMR_TIMESTAMP_TYPE uint32_t


// optional
#define MINIMR_ASSERT(x) 

// optional
#define MINIMR_DEBUGF(...) //printf(__VA_ARGS__)


// if > 0 will typedef minimr_dns_rr_a with given (max) namelen
#define MINIMR_DNS_RR_TYPE_A_DEFAULT_NAMELEN        64

// if > 0 will typedef minimr_dns_rr_aaaa with given (max) namelen
#define MINIMR_DNS_RR_TYPE_AAAA_DEFAULT_NAMELEN     64

// if > 0 will typedef minimr_dns_rr_ptr with given (max) namelen and domainlen
#define MINIMR_DNS_RR_TYPE_PTR_DEFAULT_NAMELEN      0
#define MINIMR_DNS_RR_TYPE_PTR_DEFAULT_DOMAINLEN    0

// if > 0 will typedef minimr_dns_rr_srv with given (max) namelen and targetlen
#define MINIMR_DNS_RR_TYPE_SRV_DEFAULT_NAMELEN      0
#define MINIMR_DNS_RR_TYPE_SRV_DEFAULT_TARGETLEN    0

// if > 0 will typedef minimr_dns_rr_srv with given (max) namelen and txtlen
#define MINIMR_DNS_RR_TYPE_TXT_DEFAULT_NAMELEN      0
#define MINIMR_DNS_RR_TYPE_TXT_DEFAULT_TXTTLEN      0

#endif //MINIMR_MINIMROPT_H
