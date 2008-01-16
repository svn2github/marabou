#!/usr/bin/python

import sys, ldap

try:
	l = ldap.open("edir.physik.uni-muenchen.de")
	l.protocol_version = ldap.VERSION3

except ldap.LDAPError, e:
	print "Can't open LDAP connection: ", e

if (sys.argv[1] == ''):
	filter = "cn=*"
elif (sys.argv[1] == 'L'):
	filter = "cn=*"
elif (sys.argv[1] == 'C'):
	filter = "cn=*"
else:
	filter = "cn=" + sys.argv[1]
	item = sys.argv[2];

scope = ldap.SCOPE_SUBTREE
retrieve = None

try:
	base = "ou=Campus, ou=Personen, o=Physik"
	id = l.search(base, scope, filter, retrieve)
	while 1:
		t, d = l.result(id, 0)
		if (d == []):
			break
		else:
			if (t == ldap.RES_SEARCH_ENTRY):
				for x in d[0][1]:
					if (item == x):
						print d[0][1][x][0]

except ldap.LDAPError, e:
	print "LDAP error: ", e

try:
	base = "ou=Local, ou=Personen, o=Physik"
	id = l.search(base, scope, filter, retrieve)
	while 1:
		t, d = l.result(id, 0)
		if (d == []):
			break
		else:
			if (t == ldap.RES_SEARCH_ENTRY):
				for x in d[0][1]:
					if (item == x):
						print d[0][1][x][0]

except ldap.LDAPError, e:
	print "LDAP error: ", e
