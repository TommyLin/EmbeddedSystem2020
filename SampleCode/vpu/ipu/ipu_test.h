#ifndef _IPU_TEST_H_
#define _IPU_TEST_H_

#ifdef __cplusplus
extern "C"{
#endif
#include "../include/tq_ipu.h"
#include "../include/tqvpu_lib.h"
#include "lcd.h"


int fwriten(int fd, void *vptr, size_t n);
extern void ipu_handle_init(ipu_test_handle_t *handle,struct cmd_line cmdl);
extern int ipu_test_init(struct input_argument input_argc);
extern int ipu_test_close(void);


#endif
