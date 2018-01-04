/***********************************************************************************************************************\
 * Program Name		:	main.cpp										*
 * Author		:	V. Radhakrishnan ( rk@atr-labs.com )							*
 * License		:	GPL Ver 2.0										*
 * Copyright		:	Cypress Semiconductors Inc. / ATR-LABS							*
 * Date written		:	July 7, 2012										*
 * Modification Notes	:												*
 * 															*
 * This program is the main GUI program for cyusb_suite for linux							*
\***********************************************************************************************************************/
#include <QtCore>
#include <QtGui>
#include <QtNetwork>
#include <QImage>
#include <QDateTime>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdio.h>

#include "../include/controlcenter.h"
#include "../include/cyusb.h"

ControlCenter *mainwin = NULL;
QProgressBar  *mbar = NULL;
QStatusBar *sb = NULL;
cyusb_handle  *h = NULL;
int num_devices_detected;
//int current_device_index = -1;
int current_device_index = 0;

//for camera
unsigned char image[1024*1280];
char bmpHead[0x436];
FILE * fpm;
int width = 1280;
int height = 1024;
QImage qimage(width, height, QImage::Format_RGB888);

//for image save
QDateTime dateTime;
QString dateTimeString;
QString date_file;
QDir dir("/home/yini/");   //check the file folder

static QLocalServer server(0);
static QTime *isoc_time;


extern int sigusr1_fd[2];
extern char pidfile[256];
extern char logfile[256];
extern int logfd;
extern int pidfd;

extern int fx2_ram_download(const char *filename, int extension);
extern int fx2_eeprom_download(const char *filename, int large);
extern int fx3_usbboot_download(const char *filename);
extern int fx3_i2cboot_download(const char *filename);
extern int fx3_spiboot_download(const char *filename);

struct DEVICE_SUMMARY {
	int	ifnum;
	int	altnum;
	int	epnum;
	int	eptype;
	int	maxps;
	int	interval;
};

static struct DEVICE_SUMMARY summ[100];
static int summ_count = 0;
//static unsigned int cum_data_in;
//static unsigned int cum_data_out;
//static int data_count;
//static struct libusb_transfer *transfer = NULL;
//static unsigned char *isoc_databuf = NULL;
//static int totalout, totalin, pkts_success, pkts_failure;

//static int fd_outfile, fd_infile;


static void libusb_error(int errno, const char *detailedText)
{
	char msg[30];
	char tbuf[60];

	memset(msg,'\0',30);
	memset(tbuf,'\0',60);
	QMessageBox mb;
	if ( errno == LIBUSB_ERROR_IO )
		strcpy(msg, "LIBUSB_ERROR_IO");
	else if ( errno == LIBUSB_ERROR_INVALID_PARAM )
		strcpy(msg, "LIBUSB_ERROR_INVALID_PARAM" );
	else if ( errno == LIBUSB_ERROR_ACCESS )
		strcpy(msg, "LIBUSB_ERROR_ACCESS");
	else if ( errno == LIBUSB_ERROR_NO_DEVICE )
		strcpy(msg, "LIBUSB_ERROR_NO_DEVICE");
	else if ( errno == LIBUSB_ERROR_NOT_FOUND )
		strcpy(msg, "LIBUSB_ERROR_NOT_FOUND");
	else if ( errno == LIBUSB_ERROR_BUSY )
		strcpy(msg, "LIBUSB_ERROR_BUSY");
	else if ( errno == LIBUSB_ERROR_TIMEOUT )
		strcpy(msg, "LIBUSB_ERROR_TIMEOUT");
	else if ( errno == LIBUSB_ERROR_OVERFLOW )
		strcpy(msg, "LIBUSB_ERROR_OVERFLOW");
	else if ( errno == LIBUSB_ERROR_PIPE )
		strcpy(msg, "LIBUSB_ERROR_PIPE");
	else if ( errno == LIBUSB_ERROR_INTERRUPTED )
		strcpy(msg, "LIBUSB_ERROR_INTERRUPTED");
	else if ( errno == LIBUSB_ERROR_NO_MEM )
		strcpy(msg, "LIBUSB_ERROR_NO_MEM");
	else if ( errno == LIBUSB_ERROR_NOT_SUPPORTED )
		strcpy(msg, "LIBUSB_ERROR_NOT_SUPPORTED");
	else if ( errno == LIBUSB_ERROR_OTHER )
		strcpy(msg, "LIBUSB_ERROR_OTHER");
	else strcpy(msg, "LIBUSB_ERROR_UNDOCUMENTED");

	sprintf(tbuf,"LIBUSB_ERROR NO : %d, %s",errno,msg);
	mb.setText(tbuf);
	mb.setDetailedText(detailedText);
	mb.exec();
	return;
}

static void update_devlist()
{
	int i, r, num_interfaces, index = 0;
	char tbuf[60];
	struct libusb_config_descriptor *config_desc = NULL;

	mainwin->listWidget->clear();

	for ( i = 0; i < num_devices_detected; ++i ) {
		h = cyusb_gethandle(i);
		sprintf(tbuf,"VID=%04x,PID=%04x,BusNum=%02x,Addr=%d",
				cyusb_getvendor(h), cyusb_getproduct(h),
				cyusb_get_busnumber(h), cyusb_get_devaddr(h));
		mainwin->listWidget->addItem(QString(tbuf));
		r = cyusb_get_active_config_descriptor (h, &config_desc);
		if ( r ) {
			libusb_error(r, "Error in 'get_active_config_descriptor' ");
			return;
		}
		num_interfaces = config_desc->bNumInterfaces;
		while (num_interfaces){
			if (cyusb_kernel_driver_active (h, index)){
				cyusb_detach_kernel_driver (h, index);
			}
            //add interface claim for camera image getting
            r = cyusb_claim_interface(h, index);
			index++;
			num_interfaces--;
		}
		cyusb_free_config_descriptor (config_desc);
	}
}

static void detect_device(void)
{
/*	int r;
	unsigned char byte = 0;

	r = cyusb_control_transfer(h, 0xC0, 0xA0, 0xE600, 0x00, &byte, 1, 1000);
	if ( r == 1 ) {
        mainwin->label_devtype->setText("camera type right");
        //enable_vendor_extensions();
        //mainwin->tab_4->setEnabled(TRUE);
        //mainwin->tab_5->setEnabled(FALSE);
        //mainwin->tab2->setCurrentIndex(0);
	}
	else {
        mainwin->label_devtype->setText("camera type wrong");
        //disable_vendor_extensions();
        //mainwin->rb3_custom->setChecked(TRUE);
        //mainwin->tab_4->setEnabled(FALSE);
        //mainwin->tab_5->setEnabled(TRUE);
        //mainwin->tab2->setCurrentIndex(1);
    }*/
}

/*static void check_for_kernel_driver(void)
{
    int r;
	int v = mainwin->sb_selectIf->value();

	r = cyusb_kernel_driver_active(h, v);
	if ( r == 1 ) {
		mainwin->cb_kerneldriver->setEnabled(TRUE);
		mainwin->cb_kerneldriver->setChecked(TRUE);
		mainwin->pb_kerneldetach->setEnabled(TRUE);
		mainwin->cb_kerneldriver->setEnabled(FALSE);
	}
	else {
		mainwin->cb_kerneldriver->setEnabled(FALSE);
		mainwin->cb_kerneldriver->setChecked(FALSE);
		mainwin->pb_kerneldetach->setEnabled(FALSE);
    }

}*/

/*void ControlCenter::on_pb_setAltIf_clicked()
{
    int r1, r2;
	char tval[3];
	int i = mainwin->sb_selectIf->value();
	int a = mainwin->sb_selectAIf->value();

	r1 = cyusb_claim_interface(h, i);
	if ( r1 == 0 ) {
		r2 = cyusb_set_interface_alt_setting(h, i, a);
		if ( r2 != 0 ) {
			libusb_error(r2, "Error in setting Alternate Interface");
		}
	}
	else {
		libusb_error(r1, "Error in claiming interface");
	}
	sprintf(tval,"%d",a);
	mainwin->label_aif->setText(tval);
	check_for_kernel_driver();
	update_bulk_endpoints();
    update_isoc_endpoints();
}*/

/*void ControlCenter::on_pb_setIFace_clicked()
{
    int r;
	char tval[3];
	int N, M;

	struct libusb_config_descriptor *config_desc = NULL;

	r = cyusb_get_active_config_descriptor(h, &config_desc);
	if ( r ) libusb_error(r, "Error in 'get_active_config_descriptor' ");

	N = config_desc->interface[mainwin->sb_selectIf->value()].num_altsetting;
	sprintf(tval,"%d",N);
	mainwin->le_numAlt->setText(tval);
	mainwin->sb_selectAIf->setMaximum(N - 1);
	mainwin->sb_selectAIf->setValue(0);
	mainwin->sb_selectAIf->setEnabled(TRUE);
	mainwin->pb_setAltIf->setEnabled(TRUE);
	M = mainwin->sb_selectIf->value();
	sprintf(tval,"%d",M);
	mainwin->label_if->setText(tval);
    mainwin->on_pb_setAltIf_clicked();
} */

/*static void update_summary(void)
{
    char tbuf[100];
	int i;
	char ifnum[7];
	char altnum[7];
	char epnum[7];
	char iodirn[7];
	char iotype[7];
	char maxps[7];
	char interval[7];

	ifnum[6] = altnum[6] = epnum[6] = iodirn[6] = iotype[6] = maxps[6] = interval[6] = '\0';

	memset(tbuf,'\0',100);
	mainwin->lw_summ->clear();

	for ( i = 0; i < summ_count; ++i ) {
		sprintf(ifnum,"%2d    ",summ[i].ifnum);
		sprintf(altnum,"%2d    ",summ[i].altnum);
		sprintf(epnum,"%2x    ",summ[i].epnum);
		if ( summ[i].epnum & 0x80 ) 
			strcpy(iodirn,"IN ");
		else strcpy(iodirn,"OUT");
		if ( summ[i].eptype & 0x00 )
			strcpy(iotype,"CTRL");
		else if ( summ[i].eptype & 0x01 )
			strcpy(iotype,"Isoc");
		else if ( summ[i].eptype & 0x02 )
			strcpy(iotype,"Bulk");
		else strcpy(iotype,"Intr");
		sprintf(maxps,"%4d  ",summ[i].maxps);
		sprintf(interval,"%3d   ",summ[i].interval);
		if ( i == 0 ) {
			sprintf(tbuf,"%-6s %-6s %-6s %-6s %-6s %-6s %-6s",
					ifnum,altnum,epnum,iodirn,iotype,maxps,interval);
			mainwin->lw_summ->addItem(QString(tbuf));
		}
		else {
			if ( summ[i].ifnum == summ[i-i].ifnum ) {
				memset(ifnum,' ',6);
				if ( summ[i].altnum == summ[i-1].altnum ) {
					memset(altnum,' ',6);
					sprintf(tbuf,"%-6s %-6s %-6s %-6s %-6s %-6s %-6s",
							ifnum,altnum,epnum,iodirn,iotype,maxps,interval);
					mainwin->lw_summ->addItem(QString(tbuf));
				}
				else { sprintf(tbuf,"%-6s %-6s %-6s %-6s %-6s %-6s %-6s",
						ifnum,altnum,epnum,iodirn,iotype,maxps,interval);
				mainwin->lw_summ->addItem("");
				mainwin->lw_summ->addItem(tbuf);
				}
			}
			else { sprintf(tbuf,"%-6s %-6s %-6s %-6s %-6s %-6s %-6s",
					ifnum,altnum,epnum,iodirn,iotype,maxps,interval);
			mainwin->lw_summ->addItem("");
			mainwin->lw_summ->addItem(tbuf);
			}
		}
    }
}*/


void get_config_details()
{
    /*int r;
	int i, j, k;
	char tbuf[60];
	char tval[3];
	struct libusb_config_descriptor *desc = NULL;

	h = cyusb_gethandle(current_device_index);

	r = cyusb_get_active_config_descriptor(h, &desc);
	if ( r ) {
		libusb_error(r, "Error getting configuration descriptor");
		return ;
	}
	sprintf(tval,"%d",desc->bNumInterfaces);
	mainwin->le_numIfaces->setReadOnly(TRUE);
	mainwin->le_numIfaces->setText(tval);
	mainwin->sb_selectIf->setMaximum(desc->bNumInterfaces - 1);

	sprintf(tbuf,"<CONFIGURATION>");
	mainwin->lw_desc->addItem(QString(tbuf));
	sprintf(tbuf,"bLength             = %d",   desc->bLength);
	mainwin->lw_desc->addItem(QString(tbuf));
	sprintf(tbuf,"bDescriptorType     = %d",   desc->bDescriptorType);
	mainwin->lw_desc->addItem(QString(tbuf));
	sprintf(tbuf,"TotalLength         = %d",   desc->wTotalLength);
	mainwin->lw_desc->addItem(QString(tbuf));
	sprintf(tbuf,"Num. of interfaces  = %d",   desc->bNumInterfaces);
	mainwin->lw_desc->addItem(QString(tbuf));
	sprintf(tbuf,"bConfigurationValue = %d",   desc->bConfigurationValue);
	mainwin->lw_desc->addItem(QString(tbuf));
	sprintf(tbuf,"iConfiguration      = %d",    desc->iConfiguration);
	mainwin->lw_desc->addItem(QString(tbuf));
	sprintf(tbuf,"bmAttributes        = %d",    desc->bmAttributes);
	mainwin->lw_desc->addItem(QString(tbuf));
	sprintf(tbuf,"Max Power           = %04d",  desc->MaxPower);
	mainwin->lw_desc->addItem(QString(tbuf));

	summ_count = 0;

	for ( i = 0; i < desc->bNumInterfaces; ++i ) {
		const struct libusb_interface *iface = desc->interface;
		mainwin->cb6_in->clear();
		mainwin->cb6_out->clear();
		for ( j = 0; j < iface[i].num_altsetting; ++j ) {
			sprintf(tbuf,"\t<INTERFACE>");
			mainwin->lw_desc->addItem(QString(tbuf));
			const struct libusb_interface_descriptor *ifd = iface[i].altsetting;
			sprintf(tbuf,"\tbLength             = %d",   ifd[j].bLength);
			mainwin->lw_desc->addItem(QString(tbuf));
			sprintf(tbuf,"\tbDescriptorType     = %d",   ifd[j].bDescriptorType);
			mainwin->lw_desc->addItem(QString(tbuf));
			sprintf(tbuf,"\tbInterfaceNumber    = %d",   ifd[j].bInterfaceNumber);
			mainwin->lw_desc->addItem(QString(tbuf));
			sprintf(tbuf,"\tbAlternateSetting   = %d",   ifd[j].bAlternateSetting);
			mainwin->lw_desc->addItem(QString(tbuf));
			sprintf(tbuf,"\tbNumEndpoints       = %d",   ifd[j].bNumEndpoints);
			mainwin->lw_desc->addItem(QString(tbuf));
			sprintf(tbuf,"\tbInterfaceClass     = %02x", ifd[j].bInterfaceClass);
			mainwin->lw_desc->addItem(QString(tbuf));
			sprintf(tbuf,"\tbInterfaceSubClass  = %02x", ifd[j].bInterfaceSubClass);
			mainwin->lw_desc->addItem(QString(tbuf));
			sprintf(tbuf,"\tbInterfaceProtcol   = %02x", ifd[j].bInterfaceProtocol);
			mainwin->lw_desc->addItem(QString(tbuf));
			sprintf(tbuf,"\tiInterface          = %0d",  ifd[j].iInterface);
			mainwin->lw_desc->addItem(QString(tbuf));


			for ( k = 0; k < ifd[j].bNumEndpoints; ++k ) {
				sprintf(tbuf,"\t\t<ENDPOINT>");
				mainwin->lw_desc->addItem(QString(tbuf));
				const struct libusb_endpoint_descriptor *ep = ifd[j].endpoint;
				sprintf(tbuf,"\t\tbLength             = %0d",  ep[k].bLength);
				mainwin->lw_desc->addItem(QString(tbuf));
				sprintf(tbuf,"\t\tbDescriptorType     = %0d",  ep[k].bDescriptorType);
				mainwin->lw_desc->addItem(QString(tbuf));
				sprintf(tbuf,"\t\tbEndpointAddress    = %02x", ep[k].bEndpointAddress);
				mainwin->lw_desc->addItem(QString(tbuf));
				sprintf(tbuf,"\t\tbmAttributes        = %d",   ep[k].bmAttributes);
				mainwin->lw_desc->addItem(QString(tbuf));

				summ[summ_count].ifnum    = ifd[j].bInterfaceNumber;
				summ[summ_count].altnum   = ifd[j].bAlternateSetting;
				summ[summ_count].epnum    = ep[k].bEndpointAddress;
				summ[summ_count].eptype   = ep[k].bmAttributes;
                summ[summ_count].maxps    = ep[k].wMaxPacketSize & 0x7ff;  // ignoring bits 11,12
				summ[summ_count].interval = ep[k].bInterval;
				++summ_count;

				sprintf(tbuf,"\t\twMaxPacketSize      = %04x", (ep[k].wMaxPacketSize));
				mainwin->lw_desc->addItem(QString(tbuf));
				sprintf(tbuf,"\t\tbInterval           = %d",   ep[k].bInterval);
				mainwin->lw_desc->addItem(QString(tbuf));
				sprintf(tbuf,"\t\tbRefresh            = %d",   ep[k].bRefresh);
				mainwin->lw_desc->addItem(QString(tbuf));
				sprintf(tbuf,"\t\tbSynchAddress       = %d",   ep[k].bSynchAddress);
				mainwin->lw_desc->addItem(QString(tbuf));
				sprintf(tbuf,"\t\t</ENDPOINT>");
				mainwin->lw_desc->addItem(QString(tbuf));
			}
			sprintf(tbuf,"\t</INTERFACE>");
			mainwin->lw_desc->addItem(QString(tbuf));
		}

	}
	sprintf(tbuf,"</CONFIGURATION>");
	mainwin->lw_desc->addItem(QString(tbuf));

	cyusb_free_config_descriptor(desc);

    check_for_kernel_driver();get_config_details
	update_summary();
    mainwin->on_pb_setIFace_clicked();  */
}

void get_device_details()
{
    //int width = 1280;
    //int height = 1024;
    int i = 0;
    int j = 0;
    int color = 0;
    //QImage qimage(width, height, QImage::Format_RGB888);

    //rewrite detail functions to send orders and get pictures
    int r;
    unsigned char buf[8];  //for control transfor
    int actual_length = 0;  //for bulk transfer

    //get handle
    //for(int k=0;k<10;k++)
    //{
    current_device_index = 0;   //force select
    h = cyusb_gethandle(current_device_index);
    if ( !h ) {
        printf("Error in getting a handle. curent_device_index = %d\n", current_device_index);
    }

    //claim interface
    r = cyusb_kernel_driver_active(h, current_device_index);
    if ( r != 0 ) {
        //printf("kernel driver active. Exitting\n");
        r = cyusb_detach_kernel_driver(h,current_device_index);
           //cyusb_close();
           //return 0;
        }
    r = cyusb_claim_interface(h, current_device_index);


    //send control order to get image
/*    r = cyusb_control_transfer(h,0x40,0xa8,0,0,buf,0,0);
    if( r != 0){
        printf("control order not sent.\n");
    }
    //get image by bulk transfer
    r = cyusb_bulk_transfer(h,0x82,image,1310720, &actual_length, 0);
    if(r != 0){
        printf("picture not caught");
    }

    for(i=0;i<height;i++)
    {
        for(j=0;j<width;j++)
        {
            color = image[i*width+j];
            qimage.setPixel(j,i,qRgb(color,color,color));
        }
    }

    //display image on label
    mainwin->label_image->setPixmap(QPixmap::fromImage(qimage));
    mainwin->label_image->show();  */
    //}
    //qimage.save("1.jpg");

    //
    //fpm = fopen("1.raw", "wb");
    //fwrite(image, sizeof(image), 1,fpm);
    //fclose(fpm);
}

//catch image function
void catch_image()
{
    //int width = 1280;
    //int height = 1024;
    int i = 0;
    int j = 0;
    int color = 0;
    //QImage qimage(width, height, QImage::Format_RGB888);

    //rewrite detail functions to send orders and get pictures
    int r;
    unsigned char buf[8];  //for control transfor
    int actual_length = 0;  //for bulk transfer

    //send control order to get image
    r = cyusb_control_transfer(h,0x40,0xa8,0,0,buf,0,0);
    if( r != 0){
        printf("control order not sent.\n");
    }
    //get image by bulk transfer
    r = cyusb_bulk_transfer(h,0x82,image,1310720, &actual_length, 0);
    if(r != 0){
        printf("picture not caught");
    }

    for(i=0;i<height;i++)
    {
        for(j=0;j<width;j++)
        {
            color = image[i*width+j];
            qimage.setPixel(j,i,qRgb(color,color,color));
        }
    }

    //display image on label
    mainwin->label_image->setPixmap(QPixmap::fromImage(qimage));
    mainwin->label_image->show();
    //}
    //qimage.save("1.jpg");

    //
    //fpm = fopen("1.raw", "wb");
    //fwrite(image, sizeof(image), 1,fpm);
    //fclose(fpm);
}

//add for device detection
/*void ControlCenter::on_pb_camdetail_clicked()
{
    int r;
    char tbuf[60];
    char tval[3];
    struct libusb_device_descriptor desc;
    struct libusb_config_descriptor *config_desc = NULL;

    h = cyusb_gethandle(current_device_index);
    if ( !h ) {
        printf("Error in getting a handle. curent_device_index = %d\n", current_device_index);
    }

    r = cyusb_get_device_descriptor(h, &desc);
    if ( r ) {
        libusb_error(r, "Error getting device descriptor");
        return ;
    }
    r = cyusb_get_active_config_descriptor(h, &config_desc);
    sprintf(tval,"%d",config_desc->bNumInterfaces);

    mainwin->lw_desc->clear();

    sprintf(tbuf,"<DEVICE>               ");
    mainwin->lw_desc->addItem(QString(tbuf));

    sprintf(tbuf,"bLength             = %d",   desc.bLength);
    mainwin->lw_desc->addItem(QString(tbuf));
    sprintf(tbuf,"bDescriptorType     = %d",   desc.bDescriptorType);
    mainwin->lw_desc->addItem(QString(tbuf));
    sprintf(tbuf,"bcdUSB              = %d",   desc.bcdUSB);
    mainwin->lw_desc->addItem(QString(tbuf));
    sprintf(tbuf,"bDeviceClass        = %d",   desc.bDeviceClass);
    mainwin->lw_desc->addItem(QString(tbuf));
    sprintf(tbuf,"bDeviceSubClass     = %d",   desc.bDeviceSubClass);
    mainwin->lw_desc->addItem(QString(tbuf));
    sprintf(tbuf,"bDeviceProtocol     = %d",   desc.bDeviceProtocol);
    mainwin->lw_desc->addItem(QString(tbuf));
    sprintf(tbuf,"bMaxPacketSize      = %d",   desc.bMaxPacketSize0);
    mainwin->lw_desc->addItem(QString(tbuf));
    sprintf(tbuf,"idVendor            = %04x", desc.idVendor);
    mainwin->lw_desc->addItem(QString(tbuf));
    sprintf(tbuf,"idProduct           = %04x", desc.idProduct);
    mainwin->lw_desc->addItem(QString(tbuf));
    sprintf(tbuf,"bcdDevice           = %d",   desc.bcdDevice);
    mainwin->lw_desc->addItem(QString(tbuf));
    sprintf(tbuf,"iManufacturer       = %d",   desc.iManufacturer);
    mainwin->lw_desc->addItem(QString(tbuf));
    sprintf(tbuf,"iProduct            = %d",   desc.iProduct);
    mainwin->lw_desc->addItem(QString(tbuf));
    sprintf(tbuf,"iSerialNumber       = %d",   desc.iSerialNumber);
    mainwin->lw_desc->addItem(QString(tbuf));
    sprintf(tbuf,"bNumConfigurations  = %d",   desc.bNumConfigurations);
    mainwin->lw_desc->addItem(QString(tbuf));
    sprintf(tbuf,"</DEVICE>               ");
    mainwin->lw_desc->addItem(QString(tbuf));

    //check_for_kernel_driver();
    detect_device();
    cyusb_free_config_descriptor (config_desc);
}*/

//add a button for image save
void ControlCenter::on_pb_start_clicked()
{
    //save image and display
    //get_device_details();
    catch_image();
    mainwin->pb_start->setDown(true);
  /*  int w = 1280;
    int h = 1024;
    int i = 0;
    int j = 0;
    int color = 0;

    QImage qimage(w, h, QImage::Format_RGB888);
    for(i=0;i<h;i++)
    {
        for(j=0;j<w;j++)
        {
            color = image[i*w+j];
            qimage.setPixel(j,i,qRgb(color,color,color));
        }
    }

    //display image on label
    mainwin->label_image->setPixmap(QPixmap::fromImage(qimage));
    mainwin->label_image->show();

    qimage.save("1.jpg");   */
}

//video stop
void ControlCenter::on_pb_stop_clicked()
{
    mainwin->pb_start->setDown(false);
}

//image save
void ControlCenter::on_pb_imgSave_clicked()
{
    //get system time
    dateTime = dateTime.currentDateTime();
    date_file = dateTime.toString("yyyy-MM-dd");
    date_file = "/home/yini/image_save/" + date_file;
    //dir(date_file);
    if(!dir.exists(date_file))
    {
        dir.mkpath(date_file);
        //dir.mkdir(date_file);
    }
    dateTimeString = dateTime.toString("yyyy-MM-dd_hh-mm-ss");
    qimage.save(date_file + "/" + dateTimeString + ".jpg");

}

//check this function after delete all the unused functions
static void clear_widgets()
{
    //mainwin->lw_desc->clear();
    //mainwin->label_if->clear();
    //mainwin->label_aif->clear();
    //mainwin->le_numIfaces->clear();
    //mainwin->le_numAlt->clear();
    //mainwin->sb_selectIf->clear();
    //mainwin->sb_selectIf->setEnabled(FALSE);
    //mainwin->sb_selectAIf->setValue(0);
    //mainwin->sb_selectAIf->setEnabled(FALSE);
    //mainwin->lw_desc->clear();
    //mainwin->cb_kerneldriver->setChecked(FALSE);
    //mainwin->cb_kerneldriver->setEnabled(FALSE);
    //mainwin->pb_setIFace->setEnabled(FALSE);
    //mainwin->pb_kerneldetach->setEnabled(FALSE);
    //mainwin->pb_setAltIf->setEnabled(FALSE);
    //mainwin->label_devtype->setText("");
}

static void set_if_aif()
{
/*	int r1, r2;
	char tval[5];

	int i = mainwin->sb_selectIf->value();
	int a = mainwin->sb_selectAIf->value();

	r1 = cyusb_claim_interface(h, i);
	if ( r1 == 0 ) {
		r2 = cyusb_set_interface_alt_setting(h, i, a);
		if ( r2 != 0 ) {
			libusb_error(r2, "Error in setting Alternate Interface");
			return;
		}
	}
	else {
		libusb_error(r1, "Error in setting Interface");
		return;
	}
	sprintf(tval,"%d",a);
    mainwin->label_aif->setText(tval);    */
}

//important function
void ControlCenter::on_listWidget_itemClicked(QListWidgetItem *item)
{
	item = item;
	clear_widgets();
    //current_device_index = mainwin->listWidget->currentRow();
    current_device_index = 0;
    //get_device_details();
    //get_config_details();
    //set_if_aif();
}

//from here, lots of functions are not useful
static void set_tool_tips(void)
{
    //mainwin->cb_kerneldriver->setToolTip("If checked, implies interface has been claimed");
    //mainwin->pb_kerneldetach->setToolTip("Releases a claimed interface");
    //mainwin->le3_wval->setToolTip("Enter address for A2/A3, perhaps Custom commands");
}

/*void ControlCenter::on_pb_kerneldetach_clicked()
{
	int r;

	r = cyusb_detach_kernel_driver(h, mainwin->sb_selectIf->value());
	if ( r == 0 ) {
		mainwin->cb_kerneldriver->setEnabled(TRUE);
		mainwin->cb_kerneldriver->setChecked(FALSE);
		mainwin->cb_kerneldriver->setEnabled(FALSE);
		mainwin->pb_kerneldetach->setEnabled(FALSE);
		mainwin->label_aif->clear();
	}
	else {
		libusb_error(r, "Error in detaching kernel mode driver!");
		return;
	}
}  */

void ControlCenter::sigusr1_handler()
{
	int nbr;
	char tmp;

	mainwin->sn_sigusr1->setEnabled(false);
	nbr = read(sigusr1_fd[1], &tmp, 1);

	update_devlist();
	mainwin->sn_sigusr1->setEnabled(true);	
}

static void setup_handler(int signo)
{
	int nbw;
	char a = 1;
	int N;

	printf("Signal %d (=SIGUSR1) received !\n",signo);
	cyusb_close();
	N = cyusb_open();
	if ( N < 0 ) {
		printf("Error in opening library\n");
		exit(-1);
	}
	else if ( N == 0 ) {
		printf("No device of interest found\n");
		num_devices_detected = 0;
		current_device_index = -1;
	}
	else {
		printf("No of devices of interest found = %d\n",N);
		num_devices_detected = N;
		current_device_index = 0;
	}
	nbw = write(sigusr1_fd[0], &a, 1);
}

//keep this reset function
void ControlCenter::on_pb_reset_clicked()
{
    int r;

    if ( current_device_index == -1 ) {
        QMessageBox mb;
        mb.setText("No device has been selected yet !\n");
        mb.exec();
        return ;
    }
    r = cyusb_reset_device(h);
    QMessageBox mb;
    mb.setText("Device reset");
    mb.exec();
}

/*static void dump_data(unsigned char nqimage.save("1.jpg");um_bytes, char *dbuf)
{
	int i, j, k, index, filler;
	char ttbuf[10];
	char finalbuf[256];
	char tbuf[256];
	int balance = 0;

	balance = num_bytes;
	index = 0;

	while ( balance > 0 ) {
		tbuf[0]  = '\0';
		if ( balance < 16 )
			j = balance;
		else j = 16;
		for ( i = 0; i < j; ++i ) {
			sprintf(ttbuf,"%02x ",(unsigned char)dbuf[index+i]);
			strcat(tbuf,ttbuf);
		}
		if ( balance < 16 ) {
			filler = 16-balance;
			for ( k = 0; k < filler; ++k ) 
				strcat(tbuf,"   ");
		}
		strcat(tbuf,": ");
		for ( i = 0; i < j; ++i ) {
			if ( !isprint(dbuf[index+i]) )
				strcat(tbuf,". ");
			else {
				sprintf(ttbuf,"%c ",dbuf[index+i]);
				strcat(tbuf,ttbuf);
			}
		}
		sprintf(finalbuf,"%s",tbuf);
		mainwin->lw3->addItem(finalbuf);
		balance -= j;
		index += j;
	}
}*/

//static void get_maxps(int *maxps)
//{
//	int i;
//	bool ok;
//	int iface   = mainwin->label_if->text().toInt(&ok, 10);
//	int aiface  = mainwin->label_aif->text().toInt(&ok, 10);
//
//	char found = 'n';
//	for ( i = 0; i < summ_count; ++i ) {
//	    if ( summ[i].ifnum != iface ) continue;
//	    if ( summ[i].altnum != aiface ) continue;
//	    if ( !(summ[i].eptype & 0x02 ) ) continue;
//	    if ( summ[i].epnum == mainwin->cb6_out->currentText().toInt(&ok, 16) ) {
//		found = 'y';
//		break;
//	    }
//	}
//	if ( found == 'n' ) {
//	      QMessageBox mb;
//	      mb.setText("Endpoint not found!");
//	      mb.exec();
//	      return ;
//	}
//	*maxps = summ[i].maxps;
//} 

/*static void in_callback( struct libusb_transfer *transfer)
{
    bool ok;qimage.save("1.jpg");
	int pktsin, act_len;
	char tbuf[8];
	unsigned char *ptr;
	int elapsed;
	unsigned char ep_in;
	int pktsize_in;
	char ttbuf[10];
	double inrate;

	printf("Callback function called\n");

	if ( transfer->status != LIBUSB_TRANSFER_COMPLETED ) {
		libusb_error(transfer->status, "Transfer not completed normally");
	}

	totalin = pkts_success = pkts_failure = 0;

	pktsin = mainwin->cb7_numpkts->currentText().toInt(&ok, 10);

	elapsed = isoc_time->elapsed();

	printf("Milliseconds elapsed = %d\n",elapsed);

	for ( int i = 0; i < pktsin; ++i ) {
		ptr = libusb_get_iso_packet_buffer_simple(transfer, i);
		act_len = transfer->iso_packet_desc[i].actual_length;
		totalin += 1;
		if ( transfer->iso_packet_desc[i].status == LIBUSB_TRANSFER_COMPLETED ) {
			pkts_success += 1;
			if ( mainwin->rb7_enable->isChecked() ) {
				dump_data7_in(act_len, ptr);
				mainwin->lw7_in->addItem("");
			}
		}
		else pkts_failure += 1;
	}
	sprintf(tbuf,"%6d",totalin);
	mainwin->label7_totalin->setText(tbuf);
	sprintf(tbuf,"%6d",pkts_success);
	mainwin->label7_pktsin->setText(tbuf);
	sprintf(tbuf,"%6d",pkts_failure);
	mainwin->label7_dropped_in->setText(tbuf);
	ep_in = mainwin->cb7_in->currentText().toInt(&ok, 16);  
	pktsize_in = cyusb_get_max_iso_packet_size(h, ep_in);
	inrate = ( (((double)totalin * (double)pktsize_in) / (double)elapsed ) * (1000.0 / 1024.0) );
	sprintf(ttbuf, "%8.1f", inrate);
	mainwin->label7_ratein->setText(ttbuf);
}	*/

/*static void out_callback( struct libusb_transfer *transfer)
{
	bool ok;
	int pktsout, act_len;
	char tbuf[8];
	unsigned char *ptr;
	int elapsed;
	unsigned char ep_out;
	int pktsize_out;
	char ttbuf[10];
	double outrate;


	printf("Callback function called\n");

	if ( transfer->status != LIBUSB_TRANSFER_COMPLETED ) {
		libusb_error(transfer->status, "Transfer not completed normally");
	}

	totalout = pkts_success = pkts_failure = 0;

	pktsout = mainwin->cb7_numpkts->currentText().toInt(&ok, 10);

	elapsed = isoc_time->elapsed();

	printf("Milliseconds elapsed = %d\n",elapsed);

	for ( int i = 0; i < pktsout; ++i ) {
		ptr = libusb_get_iso_packet_buffer_simple(transfer, i);
		act_len = transfer->iso_packet_desc[i].actual_length;
		totalout += 1;
		if ( transfer->iso_packet_desc[i].status == LIBUSB_TRANSFER_COMPLETED ) {
			pkts_success += 1;
			if ( mainwin->rb7_enable->isChecked() ) {
				dump_data7_out(act_len, ptr);
				mainwin->lw7_out->addItem("");
			}
		}
		else pkts_failure += 1;
	}
	sprintf(tbuf,"%6d",totalout);
	mainwin->label7_totalout->setText(tbuf);
	sprintf(tbuf,"%6d",pkts_success);
	mainwin->label7_pktsout->setText(tbuf);
	sprintf(tbuf,"%6d",pkts_failure);
	mainwin->label7_dropped_out->setText(tbuf);
	ep_out = mainwin->cb7_out->currentText().toInt(&ok, 16);  
	pktsize_out = cyusb_get_max_iso_packet_size(h, ep_out);
	outrate = ( (((double)totalout * (double)pktsize_out) / (double)elapsed ) * (1000.0 / 1024.0) );
	sprintf(ttbuf, "%8.1f", outrate);
	mainwin->label7_rateout->setText(ttbuf);
}	*/

static int multiple_instances()
{

	if ( server.listen("/dev/shm/cyusb_linux") ) {
		return 0;   /* Only one instance of this application is running  */
	}

	/* If I am here, then EITHER the application is already runnning ( most likely )
	   OR the socket file already exists because of an earlier crash ! */
	return 1;
}

void ControlCenter::appExit()
{
	exit(0);
}

void ControlCenter::about()
{
	QMessageBox mb;
    mb.setText("YYLabs Lab Cam - Version 1.0");
    mb.setDetailedText("YYlabs, Inc 2015");
	mb.exec();
	return;
}

int main(int argc, char **argv)
{
	int r;

	QApplication::setStyle(new QCleanlooksStyle);
	QApplication app(argc, argv);

	if ( multiple_instances() ) {
		printf("Application already running ? If NOT, manually delete socket file /dev/shm/cyusb_linux and restart\n");
		return -1;
	}

	r = cyusb_open();
	if ( r < 0 ) {
		printf("Error opening library\n");
		return -1;
	}
	else if ( r == 0 ) {
		printf("No device found\n");
	}
	else num_devices_detected = r;

	signal(SIGUSR1, setup_handler);

	mainwin = new ControlCenter;
	QMainWindow *mw = new QMainWindow(0);
	mw->setCentralWidget(mainwin);
	QIcon *qic = new QIcon("cypress.png");
	app.setWindowIcon(*qic);
	set_tool_tips();
    mw->setFixedSize(1300, 1000);

	update_devlist();

	sb = mw->statusBar();

	QAction *exitAct  = new QAction("e&Xit", mw);
	QAction *aboutAct = new QAction("&About", mw);

	QMenu *fileMenu = new QMenu("&File");
	QMenu *helpMenu = new QMenu("&Help");
	fileMenu->addAction(exitAct);
	helpMenu->addAction(aboutAct);
	QMenuBar *menuBar = new QMenuBar(mw);
	menuBar->addMenu(fileMenu);
	menuBar->addMenu(helpMenu);
	QObject::connect(exitAct, SIGNAL(triggered()), mainwin, SLOT(appExit()));
	QObject::connect(aboutAct,SIGNAL(triggered()), mainwin, SLOT(about()));

	mw->show();
    get_device_details();  //connect device and setup
    mainwin->pb_start->setDown(true);  //auto start

	sb->showMessage("Starting Application...",2000);

	return app.exec();
}
