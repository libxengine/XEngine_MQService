/*
 Navicat Premium Data Transfer

 Source Server         : Test
 Source Server Type    : MySQL
 Source Server Version : 80028
 Source Host           : 192.168.1.12:3306
 Source Schema         : XEngine_MQUser

 Target Server Type    : MySQL
 Target Server Version : 80028
 File Encoding         : 65001

 Date: 18/03/2022 09:16:23
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for UserInfo
-- ----------------------------
DROP TABLE IF EXISTS `UserInfo`;
CREATE TABLE `UserInfo`  (
  `ID` int NOT NULL AUTO_INCREMENT,
  `tszUserName` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT '用户名',
  `tszUserPass` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT '密码',
  `tszEMailAddr` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL COMMENT '电子邮件',
  `nPhoneNumber` bigint NULL DEFAULT NULL COMMENT '电话号码',
  `nIDNumber` bigint NULL DEFAULT NULL COMMENT '身份证',
  `nUserState` tinyint NOT NULL COMMENT '用户状态1在线,0离线',
  `nUserLevel` int NOT NULL COMMENT '用户等级,数字越小越高',
  `tszLoginTime` datetime NULL DEFAULT NULL ON UPDATE CURRENT_TIMESTAMP COMMENT '登录时间',
  `tszCreateTime` datetime NOT NULL COMMENT '注册时间',
  PRIMARY KEY (`ID`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 4 CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Table structure for UserKey
-- ----------------------------
DROP TABLE IF EXISTS `UserKey`;
CREATE TABLE `UserKey`  (
  `ID` int NOT NULL AUTO_INCREMENT,
  `tszKeyUser` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT '用户名',
  `tszKeyName` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT '用户绑定的KEY名称',
  `nKeySerial` int NOT NULL COMMENT '用户绑定KEY的序列号',
  PRIMARY KEY (`ID`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = Dynamic;

SET FOREIGN_KEY_CHECKS = 1;
