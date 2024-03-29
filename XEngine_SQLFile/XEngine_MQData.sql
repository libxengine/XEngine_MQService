/*
 Navicat Premium Data Transfer

 Source Server         : mysql
 Source Server Type    : MySQL
 Source Server Version : 80035 (8.0.35-0ubuntu0.22.04.1)
 Source Host           : 10.0.3.155:3306
 Source Schema         : XEngine_MQData

 Target Server Type    : MySQL
 Target Server Version : 80035 (8.0.35-0ubuntu0.22.04.1)
 File Encoding         : 65001

 Date: 26/12/2023 17:30:29
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for XEngine_CommKey
-- ----------------------------
DROP TABLE IF EXISTS `XEngine_CommKey`;
CREATE TABLE `XEngine_CommKey`  (
  `ID` int NOT NULL AUTO_INCREMENT,
  `tszUserName` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL COMMENT '谁发布的消息',
  `tszUserBelong` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL COMMENT '所属用户',
  `tszQueueName` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL COMMENT '所属队列',
  `nQueueSerial` bigint NOT NULL COMMENT '消息序列',
  `tszQueueLeftTime` varchar(128) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL COMMENT '过期时间',
  `tszQueuePublishTime` varchar(128) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL COMMENT '发布时间',
  `tszQueueData` longblob NOT NULL COMMENT '保存数据',
  `nDataType` tinyint NOT NULL COMMENT '数据类型',
  `nDataAttr` tinyint NULL DEFAULT NULL COMMENT '消息属性',
  `tszQueueCreateTime` datetime NOT NULL ON UPDATE CURRENT_TIMESTAMP COMMENT '插入时间',
  PRIMARY KEY (`ID`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_unicode_ci ROW_FORMAT = DYNAMIC;

SET FOREIGN_KEY_CHECKS = 1;
