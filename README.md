# Code_text_analiser_LLM
Uses LLM to access localfiles in your filesystem and read/modify them based on inputs

## Important, you must have access with a api_key and your base URL, the LLM that you'll need is more detailed in: https://openrouter.ai/docs/api/api-reference/chat/send-chat-completion-request, use a .env


#### some features:
    - Connecting to an LLM via a REST API;
    - Defining tools that the AI can use for performing tasks;
    - Implementing an agent loop that lets the AI think, act, and observe results;
    - You can create your own tools to the LMM use, just following what's already done and with the LLM documentation;
    - It has memory to use multiples tools at once, so you can delete, modify, create files with it in a single command.

## How to Use
inside de build dir, use ./code_analise -p "question" 

## How to add more tools
    - Create a new tool in the request_body
    - Create a funcion that represents that tool
    - Use the function inside of the agent loop (you need to add this informations returned by the tool in the request body)

## Compile:
    in the root:
        - cmake -S . -B build
        - cmake --build build