%%BeginOfCode%%
setenv MBS_VERSION "$mbsVersion"
setenv MARABOU_PATH "$marabouPath"
set path = ( /bin /bin/ces /usr/bin /usr/local/bin /etc /usr/etc . ~/tools $MARABOU_PATH/powerpc/bin/$mbsVersion )
source /mbs/login $mbsVersion
%%EndOfCode%%
%%MBS_LOGIN%%
