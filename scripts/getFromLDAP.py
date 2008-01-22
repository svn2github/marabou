#!/usr/bin/python

import sys, ldap

try:
	l = ldap.open("edir.physik.uni-muenchen.de")
	l.protocol_version = ldap.VERSION3

except ldap.LDAPError, e:
	print "Can't open LDAP connection: ", e

filter = "cn=*"
item = "*"

if (len(sys.argv) >= 2):
	if (sys.argv[1] == ''):
		filter = "cn=*"
	else:
		filter = "cn=" + sys.argv[1]

if (len(sys.argv) == 3):
	item = sys.argv[2]

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
				if (item == "*"):
					for it in d[0][1]:
						nx = len(d[0][1][it])
						for x in range(nx):
							print d[0][1][it][x]
				else:
					if (d[0][1].has_key(item)):
						nx = len(d[0][1][item])
						for x in range(nx):
							print d[0][1][item][x]

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
				if (item == "*"):
					for it in d[0][1]:
						nx = len(d[0][1][it])
						for x in range(nx):
							print d[0][1][it][x]
				else:
					if (d[0][1].has_key(item)):
						nx = len(d[0][1][item])
						for x in range(nx):
							print d[0][1][item][x]

except ldap.LDAPError, e:
	print "LDAP error: ", e
