/*
 Navicat Premium Data Transfer

 Source Server         : Test
 Source Server Type    : MySQL
 Source Server Version : 80028
 Source Host           : 192.168.1.12:3306
 Source Schema         : XEngine_MessageQueue

 Target Server Type    : MySQL
 Target Server Version : 80028
 File Encoding         : 65001

 Date: 15/03/2022 13:27:32
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for MessageQueue
-- ----------------------------
DROP TABLE IF EXISTS `MessageQueue`;
CREATE TABLE `MessageQueue`  (
  `ID` int NOT NULL AUTO_INCREMENT,
  `tszQueueName` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL COMMENT '所属队列',
  `nQueueSerial` bigint NULL DEFAULT NULL COMMENT '消息序列',
  `nQueueSaveTime` bigint NULL DEFAULT NULL COMMENT '保存时间',
  `nQueueGetTime` bigint NULL DEFAULT NULL COMMENT '获取时间',
  `nQueueTime` datetime NULL DEFAULT NULL COMMENT '插入时间',
  PRIMARY KEY (`ID`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = Dynamic;

SET FOREIGN_KEY_CHECKS = 1;
