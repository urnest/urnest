This directory implements some SNMP client-side functionality in clean
C++ that can be used in a multi-threaded client. (None of the classes
have locking, but they have no surprise dependencies and so can easily
be used eg with separate threads doing SNMP requests to the same or
different servers, most easily by each thread using a distinct UDP port.)

See ../snmp.hh for usage.

TODO:
  - implement all var types
  - add snmp v2c
    x SnmpV2cGetRequest, encode
    x SnmpV2cResponse, decode
    x oops revert decode(SnmpV2cResponse) and use separate
      SnmpV2cVarResponse, SnmpV2cNextVarResponse
    x validateResponse(SnmpV2cGetRequest,SnmpV2cResponse)

    x SnmpV2cSetRequest, encode
    x validateResponse(SnmpV2cSetRequest,SnmpV2cResponse)
        - test all exceptions
    - SnmpV2cGetNextRequest plus validateResponse
    - SnmpV2cTable
    - SnmpV2cWalk(?)
    - SnmpV2cGetBulk
  - server side:
    x decodeSnmpV1GetRequest
    x encodeResponse(SnmpV1GetRequest,std::vector<Value>)
    - encodeResponse(SnmpV1GetRequest,<error>)
    - decodeSnmpV1GetNextRequest
    - encodeResponse(SnmpV1GetNextRequest,std::vector<Oid,Value>)
    - encodeResponse(SnmpV1GetRequest,<error>)
    - decodeSnmpV1SetRequest
    x encodeResponse(SnmpV1SetRequest)
    x encodeResponse(SnmpV1SetRequest,<error>)

references:
  http://www.rane.com/note161.html
  https://en.wikipedia.org/wiki/X.690#BER_encoding
  https://en.wikipedia.org/wiki/X.690
  https://tools.ietf.org/html/rfc1157

notes:
  what is structure? v1 is:
    list:
      version:int
      community:string
      get/set/etc-list:
        id:int
        error:int  (0 good, other by code)
        errorIndex:int (0 none, 1 etc point refer to values)
        list:
          n x list:
            oid:oid
            value (null for get)

       some error codes:
         0x01 -- Response message too large to transport
	 0x02 -- oid not found
	 0x03 -- bad value (wrong type etc)
	 0x04 -- The SNMP manager attempted to set a read-only parameter
	 0x05 -- General Error (some error other than the ones listed above)

      - see http://www.rane.com/note161.html

   v1trap is:
     list:
      version:int
      community:string
      trap-pud-list (0xA4):
        type:oid
        sender:ipaddress
        generic:integer (enum)coldStart(0),
                         warmStart(1),
                         linkDown(2),
                         linkUp(3),
                         authenticationFailure(4),
                         egpNeighborLoss(5),
                         enterpriseSpecific(6)
        specific:int
        timestamp:timeticks
        list:
          n x list:
            oid:oid
            value

   is v2 different? RFC-1905 adds per-var error codes and bulk-pdu:
       bulk-list:
         id:int
         non-repeaters:int
         max-repeaters:int
         list:
           n x list:
             oid:oid
             value (null for get)||(noSuchObject(0)|noSuchInstance(1)|endOfMibView(2)
       value is slightly different but what does it mean? The exception codes
       can be returned with overall error==0. (And in get-next, we can
       have individual end-of-data for each column.) How are the exceptions
       encoded? Same as null, with types 0x80,0x81,0x82 which is
       CONTEXT|PRIMITIVE plus the choice number
       v2 also distinguishes between object type and instance using the last
       oid component: 0 means "the only instance", 1,2.3... means that instance
         ... the .0 convention allows us to retrieve non-table variables
             by specifying their type rather than figuring out their
             predecessor (see the RFC example)
         handling should be:
           - for tables, always drop a trailing .0 when generating the next
           oid list (but end table if all response cols end in .0)
           - but also could add an SnmpV1Walk to use for straight walks
        ... don't bother implementing get-bulk or SnmpV1Walk to start with

  encoding type,length,data:
    types (see http://www.rane.com/note161.html)
      Integer 0x02 
      Octet String 0x04
      Null 0x05
      Object Identifier 0x06
      Sequence 0x30
      GetRequest PDU 0xA0
      GetNextRequest PDU 0xA1
      GetResponse PDU 0xA2
      SetRequest PDU 0xA3
      Trap PDU 0xA4
      GetBulkRequest PDU 0xA5 (SNMPv2c)
      ... does set respond? Yes.
    
    length encoding (https://en.wikipedia.org/wiki/X.690#BER_encoding)
    use either:
      n where 0 <= n < 128
      0x80 | 1  n%256  where 128<=n<2**256
      0x80 | 2  n/(256**1)%256 n%256  where 256<=n<2**16
      0x80 | 3  n/(256**2)%256 n/(256**1)%256 n%256  where 2**16<=n<2**24
         :

    length decoding: above plus:
      indef. 0x80 <data> 0x00 0x00

    oid encoding:
       0x2B (represents .1.3)
       then words encoded as either:
         n where 0 <= n < 128
         0x80 | (n/128)%128  n % 128 where 128 <= n < 128*128
         0x80 | (n/128**2)%128**2  0x80 | (n/128)%128  n % 128 where 128 <= n < 128*128
             :
    SNMP V1 Trap uses types NetworkAdress, TimeTicks
      rfc1155 defines 
        IpAddress  [APPLICATION 0]          -- in network-byte order
                          IMPLICIT OCTET STRING (SIZE (4))
          ... application 0 means type is 0x40, network byte order
          means big-endian, or msb sent first, so 192.168.0.3 becomes
          {c0, a8, 0, 3}

        TimeTicks hundredths of a second since some epoch, ie 1=10ms, multiply
        by 10 to get milliseconds
        [APPLICATION 3]
                          IMPLICIT INTEGER (0..4294967295)

    note re ASN1 CHOICE encoding, from https://en.wikipedia.org/wiki/X.690
      The encoding of a choice value is the same as the encoding of a value 
      of the chosen type.

