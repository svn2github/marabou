/*****************************************************************************
 s_str_info.h
 *****************************************************************************
 info header of stream from SBS (s_tr_info.h from Wolfgang Ott)
 implemented for remote event server 21.3.95, Horst Goeringer
 *****************************************************************************
 */
struct s_str_info
	{
		int l_order;        /* =1, if server same type of endian */
                                    /* = 16777216 (4-byte swapped) else  */
		int l_bufsize;      /* buffer size (byte) */
		int l_num_buf;      /* number of buffers in stream */
		int l_stream_num;   /* number of active streams in SBS */
	}	;
