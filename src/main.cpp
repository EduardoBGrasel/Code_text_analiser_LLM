#include <cstdlib>
#include <iostream>
#include <string>
#include "utils.hpp"

#include <cpr/cpr.h> // C++ Requests
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main(int argc, char* argv[]) {
    // Verifies if the program is run with a question
    if (argc < 3 || std::string(argv[1]) != "-p") {
        std::cerr << "Expected first argument to be '-p'" << std::endl;
        return 1;
    }

    // get the question to be the promt to the LLM
    std::string prompt = argv[2];

    if (prompt.empty()) {
        std::cerr << "Prompt must not be empty" << std::endl;
        return 1;
    }

    // the api_key and base_url are expected do be in a .venv
    const char* api_key_env = std::getenv("OPENROUTER_API_KEY");
    const char* base_url_env = std::getenv("OPENROUTER_BASE_URL");

    // if can't find the api key is left blank
    std::string api_key = api_key_env ? api_key_env : "";
    // if can't find the base_url set the base commun base url
    std::string base_url = base_url_env ? base_url_env : "https://openrouter.ai/api/v1";

    // if the key is empty, abort
    if (api_key.empty()) {
        std::cerr << "OPENROUTER_API_KEY is not set" << std::endl;
        return 1;
    }


    // request do the LLM
    json request_body = {
        {"model", "anthropic/claude-haiku-4.5"},
        // defines that the message is send from a user
        {"messages", json::array({
            {{"role", "user"}, {"content", prompt}}
        })},
        // Tell the LLM that if need read a file, we have a funtion to do so, and it expects the filename.
        {"tools", json::array({
            {
                {"type", "function"},
                {"function", {
                    {"name", "Read"},
                    {"description", "Read and return the contents of a file"},
                    {"parameters", {
                        {"type", "object"},
                        {"properties", {
                            {"file_path", {
                                {"type", "string"},
                                {"description", "The path to the file to read"}
                            }}
                        }},
                        {"required", json::array({"file_path"})}
                    }}
                }}
            },
            {
                {"type", "function"},
                {"function", {
                    {"name", "Write"},
                    {"description", "Write content to a file"},
                    {"parameters", {
                        {"type", "object"},
                        {"properties", {
                            {"file_path", {
                                {"type", "string"},
                                {"description", "The path file to write to"}
                            }},
                            {"content", {
                                {"type", "string"},
                                {"description", "The content to write to the file"}
                            }}
                        }},
                    {"required", json::array({"file_path", "content"})}
                    }}
                }}
            },
            {
                {"type", "function"},
                {"function", {
                    {"name", "Bash"},
                    {"description", "Execute a shell command"},
                    {"parameters", {
                        {"type", "object"},
                        {"required", json::array({"command"})},
                        {"properties", {
                            {"command", {
                                {"type", "string"},
                                {"description", "the command to execute"}
                            }}
                        }}
                    }}
                }}
            }
        })}
    };

    // Agent loop
    while (true) {
        cpr::Response response = cpr::Post(
            cpr::Url{base_url + "/chat/completions"},
            cpr::Header{
                {"Authorization", "Bearer " + api_key},
                {"Content-Type", "application/json"}
            },
            cpr::Body{request_body.dump()}
        );

        if (response.status_code != 200) {
            std::cerr << "HTTP error: " << response.status_code << std::endl;
            return 1;
        }

        json result = json::parse(response.text);

        if (!result.contains("choices") || result["choices"].empty()) {
            std::cerr << "No choices in response" << std::endl;
            return 1;
        }

        // gets the message
        auto& message = result["choices"][0]["message"];
        request_body["messages"].push_back(message);
        
        // checks if the LLM wants to use a tool
        if (message.contains("tool_calls") && !message["tool_calls"].is_null()) {
            // iterates all tools that the LLM want
            for (auto& tool_call : message["tool_calls"]) {
                std::string tool_name = tool_call["function"]["name"];
                std::string call_id = tool_call["id"];

                if (tool_name == "Read") {
                    // makes the parse to find the args
                    json args = json::parse(tool_call["function"]["arguments"].get<std::string>());
                    std::string path = args["file_path"].get<std::string>();

                    std::string res = Read(path);
                    
                    request_body["messages"].push_back({
                        {"role", "tool"},
                        {"tool_call_id", call_id},
                        {"content", res}
                    });
                } else if (tool_name == "Write") {
                    // makes the parse to find the args
                    json args = json::parse(tool_call["function"]["arguments"].get<std::string>());
                    std::string path = args["file_path"].get<std::string>();
                    std::string content = args["content"].get<std::string>();

                    Write(path, content);
                    
                    request_body["messages"].push_back({
                        {"role", "tool"},
                        {"tool_call_id", call_id},
                        {"content", content}
                    }); 
                } else if (tool_name == "Bash") {
                    // makes the parse to find the args
                    json args = json::parse(tool_call["function"]["arguments"].get<std::string>());
                    std::string command = args["command"].get<std::string>();

                    std::string result = Bash_exec(command);
                    
                    request_body["messages"].push_back({
                        {"role", "tool"},
                        {"tool_call_id", call_id},
                        {"content", result}
                    }); 
                }
            }
            continue;
        } else {
            std::cout << message.value("content", "") << std::endl;
            break;
        }
}

    // You can use print statements as follows for debugging, they'll be visible when running tests.
    std::cerr << "Logs from your program will appear here!" << std::endl;

    // std::cout << result["choices"][0]["message"]["content"].get<std::string>();

    return 0;
}
