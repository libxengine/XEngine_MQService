/*
 Navicat Premium Data Transfer

 Source Server         : Test
 Source Server Type    : MySQL
 Source Server Version : 80030
 Source Host           : 192.168.1.12:3306
 Source Schema         : XEngine_MQData

 Target Server Type    : MySQL
 Target Server Version : 80030
 File Encoding         : 65001

 Date: 01/08/2022 13:15:31
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for XEngine_CommKey
-- ----------------------------
DROP TABLE IF EXISTS `XEngine_CommKey`;
CREATE TABLE `XEngine_CommKey`  (
  `ID` int NOT NULL AUTO_INCREMENT,
  `tszQueueName` varchar(255) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NOT NULL COMMENT '所属队列',
  `nQueueSerial` bigint NOT NULL COMMENT '消息序列',
  `nQueueGetTime` bigint NOT NULL COMMENT '获取次数',
  `tszQueueLeftTime` varchar(128) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT NULL COMMENT '过期时间',
  `tszQueuePublishTime` varchar(128) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT NULL COMMENT '发布时间',
  `tszQueueData` varchar(8192) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NOT NULL COMMENT '保存数据',
  `nDataLen` int NOT NULL COMMENT '数据大小',
  `tszQueueCreateTime` datetime NOT NULL ON UPDATE CURRENT_TIMESTAMP COMMENT '插入时间',
  PRIMARY KEY (`ID`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb3 COLLATE = utf8mb3_general_ci ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Table structure for XEngine_TimeRelease
-- ----------------------------
DROP TABLE IF EXISTS `XEngine_TimeRelease`;
CREATE TABLE `XEngine_TimeRelease`  (
  `tszQueueName` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '队列名称',
  `nIDMsg` bigint NOT NULL COMMENT '队列唯一ID',
  `nIDTime` bigint NOT NULL COMMENT '发布时间'
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

SET FOREIGN_KEY_CHECKS = 1;
