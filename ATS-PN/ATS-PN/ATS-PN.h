#pragma once
#include "atsplugin.h"
#include "CATSPN.h"

#define ATS_BEACON_SPDLIM 6//PN���䑬�x����
#define ATS_BEACON_STOPPATTERN 12//PN����w�ʖh�~
#define ATS_BEACON_SPDLIMD 16//PN���䑬�x��������
#define ATS_BEACON_END 91//�I�[�h��
#define ATS_BEACON_TIMETABLE 100//�����\�̕ύX
#define ATS_BEACON_HALT 105//�w�ʖh�~�`���C��
#define ATS_BEACON_LINELIMIT 130//����ō����x

int g_svcBrake;//��p�ő�
int g_emgBrake;//���u���[�L
int g_Power;//�͍s�m�b�`
int g_Brake;//�u���[�L�m�b�`
int g_Reverser;//���o�[�T
float g_TrainSpeed;//���x
int g_time;//����
int g_deltaT;//�O�̃t���[���Ƃ̎����̍�
int g_tmr;//��ԉw���_�Ń^�C�}�[
int g_StopSta;//��ԉw��
int g_AstTimer;//�A�X�^���X�N�_�Ń^�C�}�[
bool g_Aster;//�A�X�^���X�N
int g_timetable;//�����\

CATSPN g_pncontrol;

ATS_HANDLES g_output;//�o��