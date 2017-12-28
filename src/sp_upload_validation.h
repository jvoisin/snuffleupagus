#ifndef __SP_UPLOAD_VALIDATION_H__
#define __SP_UPLOAD_VALIDATION_H__

void hook_upload();

int (*sp_rfc1867_orig_callback)(unsigned int event, void *event_data,
                                void **extra);
int sp_rfc1867_callback(unsigned int event, void *event_data, void **extra);

#endif
