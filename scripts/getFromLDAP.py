#!/usr/bin/python

import sys, ldap

try:
	l = ldap.open("edir.physik.uni-muenchen.de")
	l.protocol_version = ldap.VERSION3

except ldap.LDAPError, e:
	print "Can't open LDAP connection: ", e

searchLocal = True
searchCampus = True

try:
	if (len(sys.argv) == 3):
		if (sys.argv[1] == ''):
			filter = "cn=*"
		elif (sys.argv[1] == 'L'):
			filter = "cn=*"
			searchLocal = True
			searchCampus = False
		elif (sys.argv[1] == 'C'):
			filter = "cn=*"
			searchLocal = False
			searchCampus = True
		else:
			filter = "cn=" + sys.argv[1]
			searchLocal = True
			searchCampus = True

		item = sys.argv[2];
	else:
		raise

except:
	print "Usage:    getFromLDAP.py <userName|L|C> [attribute=*]"
	print
	print "Examples: getFromLDAP.py Hans.Meier       -- get account data for specified user"
	print "          getFromLDAP.py Hans.Meier mail  -- get mail address for specified user"
	print "          getFromLDAP.py C                -- get all campus accounts"
	print
	sys.exit(1)

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
				if (d[0][1].has_key(item)):
						print d[0][1][item][0]

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
				if (d[0][1].has_key(item)):
						print d[0][1][item][0]

except ldap.LDAPError, e:
	print "LDAP error: ", e
