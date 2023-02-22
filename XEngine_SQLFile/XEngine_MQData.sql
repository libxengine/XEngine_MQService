/*
 Navicat Premium Data Transfer

 Source Server         : Test
 Source Server Type    : MySQL
 Source Server Version : 80032 (8.0.32-0ubuntu0.20.04.2)
 Source Host           : 192.168.1.10:3306
 Source Schema         : XEngine_MQData

 Target Server Type    : MySQL
 Target Server Version : 80032 (8.0.32-0ubuntu0.20.04.2)
 File Encoding         : 65001

 Date: 22/02/2023 16:11:34
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for XEngine_CommKey
-- ----------------------------
DROP TABLE IF EXISTS `XEngine_CommKey`;
CREATE TABLE `XEngine_CommKey`  (
  `ID` int NOT NULL AUTO_INCREMENT,
  `tszUserName` varchar(255) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NOT NULL COMMENT '谁发布的消息',
  `tszQueueName` varchar(255) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NOT NULL COMMENT '所属队列',
  `nQueueSerial` bigint NOT NULL COMMENT '消息序列',
  `nQueueGetTime` bigint NOT NULL COMMENT '获取次数',
  `tszQueueLeftTime` varchar(128) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT NULL COMMENT '过期时间',
  `tszQueuePublishTime` varchar(128) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT NULL COMMENT '发布时间',
  `tszQueueData` varchar(8192) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NOT NULL COMMENT '保存数据',
  `nDataLen` int NOT NULL COMMENT '数据大小',
  `nDataType` tinyint NULL DEFAULT NULL COMMENT '数据类型',
  `tszQueueCreateTime` datetime NOT NULL ON UPDATE CURRENT_TIMESTAMP COMMENT '插入时间',
  PRIMARY KEY (`ID`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb3 COLLATE = utf8mb3_general_ci ROW_FORMAT = DYNAMIC;

SET FOREIGN_KEY_CHECKS = 1;
