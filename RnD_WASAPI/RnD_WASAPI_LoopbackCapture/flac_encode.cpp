#include "FLAC/stream_encoder.h"

#define __FLAC_OPTION_FUCTION 0

static FLAC__StreamEncoderWriteStatus stream_encoder_write_callback_(const FLAC__StreamEncoder *encoder, const FLAC__byte buffer[], size_t bytes, unsigned samples, unsigned current_frame, void *client_data)
{

	// �Ű����� `sample`, `current_frame`�� �����뵵
	// �Ű����� `client_data`�� `FLAC__stream_encoder_init_stream(...,void *client_data)���� �������� �������̴�

	//TODO: ���⿡�� �����͸� �������ϴ°� ����
	FILE *f = (FILE*)client_data;
	(void)encoder, (void)samples, (void)current_frame;
	if (fwrite(buffer, 1, bytes, f) != bytes)
		return FLAC__STREAM_ENCODER_WRITE_STATUS_FATAL_ERROR;
	else
		return FLAC__STREAM_ENCODER_WRITE_STATUS_OK;
}

int flac_encode(char *wasapi_pData, unsigned int wasapi_size) {

	//TODO: channels, bps, samplerate ���� �Ű������� �Ѱ��൵ ������ �� ����


	FLAC__StreamEncoder *encoder = 0;

	FLAC__bool hs;
	FLAC__StreamEncoderInitStatus init_status;

	unsigned channels = 0;
	unsigned bps = 0;
	unsigned sample_rate = 0;

	/* allocate the encoder */
	if ((encoder = FLAC__stream_encoder_new()) == NULL) {
		fprintf(stderr, "ERROR: allocating encoder\n");
		return -1;
	}

	/* at minimum default settings functions */
	channels = 2;
	bps = 16;
	sample_rate = 48000;

	if (!FLAC__stream_encoder_set_channels(encoder, channels))
		return -1;
	printf("set_channels OK\n");

	if (!FLAC__stream_encoder_set_bits_per_sample(encoder, bps))
		return -1;
	printf("set_bps(bits per sample) OK\n");

	if (!FLAC__stream_encoder_set_sample_rate(encoder, sample_rate))
		return -1;
	printf("set_sample_rate OK\n");

	//if you unknown estimate of the total samples than set to 0 (I think this function when encode file)
	if (!FLAC__stream_encoder_set_total_samples_estimate(encoder, wasapi_size))
		return -1;
	printf("set_total_samples_estimate OK\n");

#if __FLAC_OPTION_FUCTION
	/* option for settings functions */
	if (!FLAC__stream_encoder_set_verify(encoder, true))
		return -1;
	printf("set_verify OK\n");

	//if you not call this function than compresstion level is 5 (0~8)
	if (!FLAC__stream_encoder_set_compression_level(encoder, (unsigned)(-1)))
		return -1;
	printf("set_comression_level OK\n");
#endif

	/* prepare for encoding */
	init_status = FLAC__stream_encoder_init_stream(encoder, stream_encoder_write_callback_, /*seek_callback=*/0, /*tell_callback=*/0, /*meta_callback=*/0, /*client_data=*/wasapi_pData);
	if (init_status != FLAC__STREAM_ENCODER_INIT_STATUS_OK) {
		printf("Encoder Init Failed\n");
		return -1;
	}

	/*if (!FLAC__stream_encoder_process_interleaved(encoder, samples, sizeof(samples) / sizeof(FLAC__int32)))
		return -1;
	printf("OK\n");*/





	if (!FLAC__stream_encoder_finish(encoder))
		return -1;
	printf("finsih OK\n");

	FLAC__stream_encoder_delete(encoder);
	printf("delete OK\n");

	return 0;
}