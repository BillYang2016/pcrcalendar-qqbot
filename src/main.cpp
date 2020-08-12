#pragma once

#include <iostream>
#include <fstream>
#include <set>
#include <sstream>
#include <direct.h>
#include <io.h>

#include <cqcppsdk/cqcppsdk.h>
#include <yaml-cpp/yaml.h>
#include "json.hpp"

#ifndef HEADERS

using json = nlohmann::json;
using node = YAML::Node;

using cq::utils::ansi;

using namespace cq;
using namespace std;
using Message = cq::message::Message;
using MessageSegment = cq::message::MessageSegment;

#define HEADERS

#else

#define HEADERS extern

#endif

#include "datagenerator.hpp"
#include "querydata.hpp"
#include "register.hpp"
#include "stringsplit.hpp"

CQ_INIT {
    on_enable([] { 
        logging::info("启用", "公主连结黄历已启用");
        Generate();
    });

    on_group_message([](const GroupMessageEvent &event) {
        string yml = ansi(dir::app()+"config.yml");
        node config;

        vector<string> groups;
        try { //读取配置
            config = YAML::LoadFile(yml);
            string string_groups=config["groups"].as<string>();
            groups=stringSplit(string_groups,",");
        } catch (ApiError &err) {
            logging::warning("加载数据","读取配置失败！错误码："+to_string(err.code));
        }

        if(config["enable"].as<string>()!="true")return; //插件未启用
        
        bool ENABLED=0;
        for(auto group:groups)if(group==to_string(event.group_id)) {ENABLED=1;break;}
        if (ENABLED == 0) return; // 不在启用的群中, 忽略
        
        string command1,command2;

        try { //读取配置
            command1=config["command"]["register"].as<string>();
            command2=config["command"]["query"].as<string>();
        } catch (ApiError &err) {
            logging::warning("加载数据","读取命令失败！错误码："+to_string(err.code));
        }

        if(event.message!=command1&&event.message!=command2)return; //不触发指令

        if(event.message==command1) { //签到

            tm *t;
            int lastyear,nowyear,lastmonth,nowmonth,lastday,nowday;
            json data;

            try { //读取签到数据
                ifstream jsonFile(ansi(dir::app()+"groups\\"+to_string(event.group_id)+".json"));
                data=json::parse(jsonFile);
                json user=data[to_string(event.user_id)];
                time_t lastt=user["lasttime"].get<time_t>(),nowt=time(NULL);
                t=localtime(&lastt);
                lastyear=t->tm_year,lastmonth=t->tm_mon,lastday=t->tm_mday;
                t=localtime(&nowt);
                nowyear=t->tm_year,nowmonth=t->tm_mon,nowday=t->tm_mday;
            } catch (ApiError &err) {
                logging::warning("加载数据","读取签到数据失败！错误码："+to_string(err.code));
                return;
            } catch (nlohmann::detail::parse_error &err) { //json不存在
                logging::info("加载数据","json数据不存在，重新创建");
                _mkdir(ansi(dir::app()+"groups\\").c_str());
                ofstream os(ansi(dir::app()+"groups\\"+to_string(event.group_id)+".json"));
                data["readme"]="Do not modify this file unless you know what you're doing!"; //readme
                os << data.dump(4) << endl;
                os.close();
                time_t lastt=1000000000,nowt=time(NULL);
                t=localtime(&lastt);
                lastyear=t->tm_year,lastmonth=t->tm_mon,lastday=t->tm_mday;
                t=localtime(&nowt);
                nowyear=t->tm_year,nowmonth=t->tm_mon,nowday=t->tm_mday;
            } catch (nlohmann::detail::type_error &err) { //json没有用户签到数据
                logging::info("加载数据","json数据不存在");
                time_t lastt=1000000000,nowt=time(NULL);
                t=localtime(&lastt);
                lastyear=t->tm_year,lastmonth=t->tm_mon,lastday=t->tm_mday;
                t=localtime(&nowt);
                nowyear=t->tm_year,nowmonth=t->tm_mon,nowday=t->tm_mday;
            }

            try {
                if(lastyear==nowyear && lastmonth==nowmonth && lastday==nowday) { //今天已经签过到了
                    send_group_message(event.group_id,MessageSegment::at(event.user_id)+"您今天已经签过到啦！");
                    logging::info("签到",to_string(event.user_id)+"重复签到");
                } else {
                    json result;
                    if(Register(event.group_id,event.user_id,result)) {
                        ofstream os(ansi(dir::app()+"groups\\"+to_string(event.group_id)+".json"));
                        json user=result;
                        user["lasttime"]=time(NULL);
                        data[to_string(event.user_id)]=user;
                        os << data.dump(4) << endl;
                        os.close();
                    }
                    logging::info("签到",to_string(event.user_id)+"成功签到");
                }
            } catch (ApiError &err) {} //忽略错误

        } else { //查询

            if(!QueryData(event.group_id,event.user_id)) {
                send_group_message(event.group_id,MessageSegment::at(event.user_id)+"您今天尚未签到！");
                logging::info("查询运势",to_string(event.user_id)+"未签到");
            }

        }

    });
}
