#include "stdio.h"
#include "mbsio.h"
#include "expIndices.h"

#define TRUE	1
#define FALSE	0

enum	{ kMaxErrors	=	10	};

unsigned int mbs_next_sheader(const MBSDataIO *);
unsigned int mbs_next_sdata(const MBSDataIO *);
const MBSDataIO * mbs_open_file(const char *, const char *, int, FILE *);
int mbs_close_file(const MBSDataIO *);
unsigned int mbs_next_event(const MBSDataIO *);

int main(int argc, char * argv[]) {
	if (argc <= 1) {
		printf("Usage: ./rmbs <medfile>\n");
	} else if (argc == 2) {
		readMbsData(argv[1], 0);
	} else {
		readMbsData(argv[1], atoi(argv[2]));
	}
}

int readMbsData(char * fileName, int maxEvents) {

	int i;
	const MBSDataIO * mbs;
	unsigned int eventType = 0;
	int abortOnError = 0;
	int nofErrors = 0;
	int nofEvents = 0;

	if (maxEvents == 0) maxEvents = 1000000000;

	mbs = mbs_open_file(fileName, "F", 0x4000, NULL);			/* open file (ext = .lmd or .med) */

	if (mbs != NULL) {
		for (i = 0; i < maxEvents; i++) {						/* loop over events */
			eventType = mbs_next_event(mbs);					/* next event */
			if (eventType == MBS_ETYPE_EOF) {					/* end of file? */
				printf("End of file %s\n", fileName);
				mbs_close_file(mbs);
				break;
			} else if (eventType == MBS_ETYPE_ERROR) {			/* error? */
				nofEvents++;
				nofErrors++;
				abortOnError++;
				if (abortOnError > kMaxErrors) {
					printf("Aborting after %d subsequent errors\n", abortOnError);
					eventType = MBS_ETYPE_ABORT;
         			break;
				}
			} else if (eventType == MBS_ETYPE_ABORT) {			/* abort? */
				nofEvents++;
				nofErrors++;
				printf("Aborting\n");
				break;
			} else {
				nofEvents++;
				if (processEvent(mbs) != TRUE) {				/* process event */
					nofErrors++;
					eventType = MBS_ETYPE_ERROR;
					break;
				}
				abortOnError = 0;
			}
		}
		printf("Processed %d events: %d error(s)\n", nofEvents, nofErrors);
	}
	exit(0);
}

int processEvent(MBSDataIO * mbs) {

	int sts;
	unsigned int sevtPattern;

	s_vehe * eventData = (s_vehe *) mbs->evt_data; 		/* pointer to event data */

	switch (mbs_get_event_trigger(mbs)) {		/* dispatch according to trigger number */
		case kMrbTriggerReadout:				/* event readout, trigger 1 */
			sts = extractSubevents(mbs);		/* extract subevents */
			if (sts == FALSE) return(FALSE);
			userCode(); 						/* =========== user's event processing goes here ========================= */
			return(TRUE);			
		case kMrbTriggerStartAcquisition:		/* start acquisition (trigger 14) */
			printf("Start trigger #14\n");
			return(TRUE);

		case kMrbTriggerStopAcquisition:		/* stop acquisition (trigger 15) */
			printf("Stop trigger #15\n");
			return(TRUE);

		default:
			printf("Illegal trigger number - #%d\n", eventData->i_trigger);
			return(FALSE);
	}
}

int extractSubevents(MBSDataIO * mbs) {
	int sts;
	unsigned int sevtType;
	unsigned short * sevtData;
	unsigned short sevtPattern = 0;

	for (;;) {
		sevtType = mbs_next_sheader(mbs);
		if (sevtType == MBS_STYPE_EOE) return(TRUE);		/* end of event */
		if (sevtType == MBS_STYPE_ABORT) return(FALSE);	/* error */

		sevtType = mbs_next_sdata(mbs); 					/* next subevent */
		if (sevtType == MBS_STYPE_ERROR) return(FALSE);	/* error */

		switch (mbs_get_sevent_serial()) {		/* dispatch according to subevent id */
			case kMrbSevtClu1:				/* subevent clu1: dgf cluster 1, id=1 */
			case kMrbSevtClu2:				/* subevent clu2: dgf cluster 2, id=2 */
			case kMrbSevtClu3:				/* subevent clu3: dgf cluster 3, id=3 */
			case kMrbSevtClu4:				/* subevent clu4: dgf cluster 4, id=4 */
			case kMrbSevtClu5:				/* subevent clu5: dgf cluster 5, id=5 */
			case kMrbSevtClu6:				/* subevent clu6: dgf cluster 6, id=6 */
			case kMrbSevtClu7:				/* subevent clu7: dgf cluster 7, id=7 */
			case kMrbSevtClu8:				/* subevent clu8: dgf cluster 8, id=8 */
			case kMrbSevtClu9:				/* subevent clu9: dgf cluster 9 (time stamping dgf), id=9 */
			case kMrbSevtClu10:				/* subevent clu10: dgf cluster 10 (beam dump dgf), id=10 */
				sts = decodeDgfData(mbs);
				if (sts == FALSE) return(FALSE);
				continue;
 			case kMrbSevtCdf1:				/* caen adc data id=11 */
 			case kMrbSevtCdf2:				/* caen adc data id=12 */
 			case kMrbSevtCdb1:				/* caen adc data id=13 */
 			case kMrbSevtCdb2:				/* caen adc data id=14 */
 			case kMrbSevtCdb3:				/* caen adc data id=15 */
 			case kMrbSevtCde:				/* caen adc data id=16 */
				sts = decodeCaenAdcData(mbs);
				if (sts == FALSE) return(FALSE);
				continue;
			case kMrbSevtCdt1:				/* caen tdc data, id=17 */
			case kMrbSevtCdt2:				/* caen tdc data, id=18 */
				sts = decodeCaenTdcData(mbs);
				if (sts == FALSE) return(FALSE);
				continue;
			case kMrbSevtScData:			/* subevent scData: scaler data, id=19 */
				sts = decodeScalerData(mbs);
				if (sts == FALSE) return(FALSE);
				continue;
			case kMrbSevtPuData:			/* subevent puData: pattern unit id=20 */
				sts = decodePuData(mbs);
				if (sts == FALSE) return(FALSE);
				continue;
			case kMrbSevtDsca1:				/* subevent dsca1: dgf counters crate 1, id=21 */
			case kMrbSevtDsca2:				/* subevent dsca2: dgf counters crate 2, id=22 */
			case kMrbSevtDsca3:				/* subevent dsca3: dgf counters crate 3, id=23 */
				sts = decodeDgfCounters(mbs);
				if (sts == FALSE) return(FALSE);
				continue;

			case kMrbSevtTimeStamp: 		/* subevent type [9000,1] (ROOT Id=999): time stamp */
				continue;

			case kMrbSevtDeadTime:			/* subevent type [9000,2] (ROOT Id=998): dead time data */
				continue;
				
			case kMrbSevtDummy: 			/* dummy subevent created by MBS internally [111,111] (ROOT Id=888), wc = 0 */
				continue;

			default: 						/* unknown subevent id */
				printf("Unknown subevent id - %d\n", mbs->sevt_id);
				continue;
		}
	}
}

/* subevent id 1-10, type [10,22]: raw dgf data, format 0x101 */
int decodeDgfData(MBSDataIO * mbs) {
	int sevtId;
	int sevtWc;
	unsigned short * sevtData;
	sevtId = mbs_get_sevent_serial(mbs);
	sevtWc = mbs_get_sevent_wc(mbs);
	sevtData = mbs_get_sevent_dataptr(mbs);
	printf("Subevent %d: Dgf data, wc=%d\n", sevtId, sevtWc);
	return(TRUE);
}

/* subevent id 11-16, type [10,42]: raw caen data */
int decodeCaenAdcData(MBSDataIO * mbs) {
	int sevtId;
	int sevtWc;
	unsigned short * sevtData;
	sevtId = mbs_get_sevent_serial(mbs);
	sevtWc = mbs_get_sevent_wc(mbs);
	sevtData = mbs_get_sevent_dataptr(mbs);
	printf("Subevent %d: Caen V785 adc data, wc=%d\n", sevtId, sevtWc);
	return(TRUE);
}

/* subevent id 17 + 18, type [10,42]: raw caen data */
int decodeCaenTdcData(MBSDataIO * mbs) {
	int sevtId;
	int sevtWc;
	unsigned short * sevtData;
	sevtId = mbs_get_sevent_serial(mbs);
	sevtWc = mbs_get_sevent_wc(mbs);
	sevtData = mbs_get_sevent_dataptr(mbs);
	printf("Subevent %d: Caen V775 tdc data, wc=%d\n", sevtId, sevtWc);
	return(TRUE);
}

/* subevent 19, type [10,11]: scaler data: 3 scalers a 32 chn a 32 bit */
int decodeScalerData(MBSDataIO * mbs) {
	int sevtId;
	int sevtWc;
	unsigned short * sevtData;
	sevtId = mbs_get_sevent_serial(mbs);
	sevtWc = mbs_get_sevent_wc(mbs);
	sevtData = mbs_get_sevent_dataptr(mbs);
	printf("Subevent %d: scaler data, wc=%d\n", sevtId, sevtWc);
	return(TRUE);
}

/* subevent 20, type [10,53]: pattern unit Sis3600 */
int decodePuData(MBSDataIO * mbs) {
	int sevtId;
	int sevtWc;
	unsigned short * sevtData;
	sevtId = mbs_get_sevent_serial(mbs);
	sevtWc = mbs_get_sevent_wc(mbs);
	sevtData = mbs_get_sevent_dataptr(mbs);
	printf("Subevent %d: scaler data, wc=%d\n", sevtId, sevtWc);
	return(TRUE);
}

/* subevent 21, 22, 23, type [10,65]: internal dgf counters:         */
/*                                    0: magic word 0x2525           */
/*                                    1: word count                  */
/*                                    2: subevent id                 */
/*                                    3: module number               */
/*                                    4: data (RealTimeA) ...        */
/*                                    N: ... (FastPeaksB3)           */
/*                                    N+1: magic word                */
/*                                    ... ... ...                    */
/*                                    X: end of data 0x5252          */
int decodeDgfCounters(MBSDataIO * mbs) {
	int sevtId;
	int sevtWc;
	unsigned short * sevtData;
	sevtId = mbs_get_sevent_serial(mbs);
	sevtWc = mbs_get_sevent_wc(mbs);
	sevtData = mbs_get_sevent_dataptr(mbs);
	printf("Subevent %d: internal dgf counters, wc=%d\n", sevtId, sevtWc);
	return(TRUE);
}

void userCode() {}		/* user's event processing */
