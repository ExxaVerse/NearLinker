#!/bin/bash

#CA
openssl genrsa -out ca_key.pem 2048
openssl req -new -x509 -key ca_key.pem -out ca_cert.pem -config ca.cnf -days 9999
openssl ca -config ca.cnf -gencrl -out ca_crl.pem

#Server certificate
openssl genrsa -out key.pem
openssl req -new -sha256 -key key.pem -out csr.pem -config server.cnf
openssl x509 -req -days 9999 -in csr.pem -out cert.pem -CA ca_cert.pem -CAkey ca_key.pem -CAcreateserial -extensions req_extensions -extfile server.cnf -sha256

#Check
#openssl s_server -key key.pem -cert cert.pem -accept 3000 -www
#openssl s_client -CAfile ca_cert.pem -connect localhost:3000 -state

#Copy to UE
cp ca_cert.pem ../../nl-ue4-example-project/Content/Certificates/cacert.pem

#Generate pin key
#openssl x509 -pubkey -noout -in cert.pem  > pubkey.pem
#cat pubkey.pem | openssl pkey -pubin -outform der | openssl dgst -sha256 -binary | openssl enc -base64 > pubkey.pin
