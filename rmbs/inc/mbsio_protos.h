#include <Rtypes.h>

//_________________________________________________________[C DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           mbsio_protos.h
// Purpose:        C interface to mbsio calls
// Description:    C prototypes for mbsio
// Author:         R. Lutter
// Revision:       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	extern "C" void mbs_set_sevt_minwc(const MBSDataIO *, Int_t);
	extern "C" UInt_t mbs_next_sevent_raw(const MBSDataIO *);
	extern "C" UInt_t mbs_next_sevent(const MBSDataIO *);
	extern "C" UInt_t mbs_next_sheader(const MBSDataIO *);
	extern "C" UInt_t mbs_get_sevent_subtype(const MBSDataIO *);
	extern "C" Int_t mbs_get_sevent_serial(const MBSDataIO *);
	extern "C" Int_t mbs_get_sevent_wc(const MBSDataIO *);
	extern "C" UShort_t * mbs_get_sevent_dataptr(const MBSDataIO *);
	extern "C" UInt_t mbs_next_sdata_raw(const MBSDataIO *);
	extern "C" UInt_t mbs_next_sdata(const MBSDataIO *);
	extern "C" void mbs_pass_errors(const Char_t *);
	extern "C" const MBSDataIO * mbs_open_file(const Char_t *, const Char_t *, Int_t, FILE *);
	extern "C" Bool_t mbs_close_file(const MBSDataIO *);
	extern "C" Bool_t mbs_free_dbase(const MBSDataIO *);
	extern "C" UInt_t mbs_next_event(const MBSDataIO *);
	extern "C" Int_t mbs_get_event_trigger(const MBSDataIO *);
	extern "C" Int_t mbs_pass_sevent(const MBSDataIO * , UShort_t *);
	extern "C" Int_t mbs_show(const MBSDataIO *, const Char_t *, FILE *);
	extern "C" Int_t mbs_set_show(const MBSDataIO *, const Char_t *, Int_t , FILE *);
	extern "C" Int_t mbs_show_stat(const MBSDataIO *, FILE *);
	extern "C" Int_t mbs_set_stat(const MBSDataIO *, Int_t, FILE *);
	extern "C" Int_t mbs_set_stream(const MBSDataIO *, Int_t, Int_t);
	extern "C" Int_t mbs_set_dump(const MBSDataIO *, Int_t);
	extern "C" Int_t mbs_open_log(const Char_t *);
	extern "C" Int_t mbs_open_lmd(const Char_t *);
	extern "C" Int_t mbs_close_lmd();
	extern "C" Int_t mbs_open_med(const Char_t *);
	extern "C" Int_t mbs_close_med();

