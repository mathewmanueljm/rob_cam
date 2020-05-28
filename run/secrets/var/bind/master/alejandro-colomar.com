$TTL										604800
@		IN	SOA	dns.alejandro-colomar.com. root.dns.alejandro-colomar.com. (
										2
										604800
										86400
										2419200
										604800 )
		IN	NS	dns.alejandro-colomar.com.
dns		IN	A	127.0.0.1
kube-apiserver	IN	CNAME	dns
robot		IN	A	10.168.6.100
