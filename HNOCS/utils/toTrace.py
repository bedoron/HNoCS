#!/usr/bin/python
import json
import sys

fname="responseDB_by_router.txt"
REQUEST=0
RESPONSE=1
path = sys.argv[1]+'/'

def print_rec(record, typeOf):
	fmt = "%s, %d, %d, %d, % .12e"
	print record['router_id'],
	if typeOf==REQUEST:
		print ", request, "+fmt%(record['session_type'], record['session_id'], record['response_out_port'], record['request_out_port'], record['request_ts'])
	else:
		print ", response, "+fmt%(record['session_type'], record['session_id'], record['request_out_port'], record['response_out_port'], record['response_ts'])


read_data = ""
with open(path+fname, 'r') as f:
	read_data = f.read()

routers_db = json.loads(read_data)


while routers_db:
	router_zero_by_req = sorted(routers_db[0], key=lambda x: x['request_ts'])
	router_zero_by_res = sorted(routers_db[0], key=lambda x: x['response_ts'])

	
	while router_zero_by_req and router_zero_by_res:	
		if router_zero_by_res[0]['response_ts']<router_zero_by_req[0]['request_ts']:
			print_rec(router_zero_by_res[0], RESPONSE)
			router_zero_by_res.pop(0)
		else:
			print_rec(router_zero_by_req[0], REQUEST)
			router_zero_by_req.pop(0)

	if len(router_zero_by_req)>0:
		print_rec(router_zero_by_req[0], REQUEST)
		router_zero_by_req.pop(0)

	if len(router_zero_by_res)>0:
		print_rec(router_zero_by_res[0], RESPONSE)
		router_zero_by_res.pop(0)

	routers_db.pop(0)




