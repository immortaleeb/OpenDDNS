/**
 * A collection of some codes and flags that will be used in this application.
 * for the DNS messages.
 * Based on a comprehensive DNS overview: http://www.zytrax.com/books/dns/ch15/
 * A full list can be found here: http://www.iana.org/assignments/dns-parameters/dns-parameters.xhtml
 */

#ifndef CODES_H_
#define CODES_H_

// 4 bit, Used for OPCODE in header
#define OPCODE_QUERY 0
#define OPCODE_IQUERY 1
#define OPCODE_STATUS 2

// 4 bit
#define RCODE_NOERROR 0
#define RCODE_FORMATERROR 1
#define RCODE_SERVERFAILURE 2
#define RCODE_NAMEERROR 3
#define RCODE_NOTIMPLEMENTED 4
#define RCODE_REFUSED 5

// Unsigned 16 bit, QTYPE in questions or TYPE in answers (resource records)
// For now, we only allow A queries.
#define RR_TYPE_A 1
/*
#define RR_TYPE_NS 2
#define RR_TYPE_CNAME 5
#define RR_TYPE_SOA 6
#define RR_TYPE_WKS 11
#define RR_TYPE_PTR 12
#define RR_TYPE_MX 15
#define RR_TYPE_SRV 33
#define RR_TYPE_A6 38
*/
#define RR_TYPE_ANY 255

// Sizes are in bytes
#define RR_TYPE_A_SIZE 4

// Unsigned 16 bit, CLASS in answers (resource records)
#define RR_CLASS_IN 1

#endif /* CODES_H_ */
