-- Groq Translation Plugin
-- Uses Groq API to translate text.
-- Requires an API Key.

-- Metadata
-- Name: Groq Translator
-- Version: 1.0

-- Configuration
-- API Key is now retrieved from settings
local MODEL = "llama-3.3-70b-versatile"

function on_define_settings()
    return {
        {
            key = "api_key",
            label = "Groq API Key",
            type = "password",
            default = ""
        },
        {
            key = "model",
            label = "Model Name",
            type = "dropdown",
            default = "llama-3.3-70b-versatile",
            options = {
                "llama-3.3-70b-versatile",
                "llama-3.1-8b-instant",
                "llama-3.2-90b-vision-preview",
                "gemma2-9b-it",
                "mixtral-8x7b-32768",
                "qwen/qwen3-32b",
                "allam-2-7b"
            }
        }
    }
end


-- Helper function for API requests with retry logic
function request_with_retry(url, method, headers, body)
    local max_retries = 5
    local attempt = 0
    local backoff_delay = 2000 -- Start with 2 seconds

    while attempt <= max_retries do
        attempt = attempt + 1
        nst_log("[DEBUG] Request attempt " .. attempt .. " of " .. (max_retries + 1))
        
        local response_body, status, response_headers = nst_http_request(url, method, headers, body)
        
        if status == 200 then
            return response_body, status, response_headers
        elseif status == 429 then
            nst_log("[WARNING] Rate limit reached (429).")
            
            local wait_time = backoff_delay
            
            -- Try to parse Retry-After header
            if response_headers and type(response_headers) == "table" then
                -- Header keys might be case-insensitive or normalized, usually lower case in my C++ impl? 
                -- Actually QNetworkReply::rawHeaderList returns original case usually, but I didn't normalize.
                -- Let's check a few variations or iterate.
                local retry_after = nil
                for k, v in pairs(response_headers) do
                    if string.lower(k) == "retry-after" then
                        retry_after = tonumber(v)
                        break
                    end
                end
                
                if retry_after then
                    nst_log("[DEBUG] Found Retry-After header: " .. retry_after .. " seconds")
                    wait_time = retry_after * 1000 -- Convert to ms
                end
            end
            
            if attempt > max_retries then
                return response_body, status, "Rate limit exceeded after max retries."
            end
            
            nst_log("[INFO] Waiting " .. (wait_time/1000) .. "s before retrying...")
            
            if nst_sleep then
                nst_sleep(wait_time)
            else
                nst_log("[WARNING] nst_sleep not available, cannot wait properly. Aborting retry.")
                return response_body, status, "Rate limit reached (nst_sleep missing)"
            end
            
            -- Increase backoff for next time (exponential)
            backoff_delay = backoff_delay * 2
        else
            -- Other errors
            return response_body, status, nil
        end
    end
    return nil, 0, "Unknown error" 
end

function on_text_extract(text)
    nst_log("[DEBUG] on_text_extract called with text: " .. text)
    local api_key = nst_get_setting("api_key")
    local model = nst_get_setting("model")
    nst_log("[DEBUG] Retrieved settings - api_key: " .. (api_key or "nil") .. ", model: " .. (model or "nil"))
    
    if not api_key or api_key == "" then
        -- Fallback to env var for backward compatibility or testing
        api_key = os.getenv("GROQ_API_KEY")
        nst_log("[DEBUG] Using env var for API key: " .. (api_key or "nil"))
    end

    if not api_key or api_key == "" then
        local msg = "Error: Groq API Key not set in settings."
        nst_log(msg)
        return nil, msg
    end
    
    if not model or model == "" then model = MODEL end

    nst_log("Translating with Groq (" .. model .. "): " .. text)

    local url = "https://api.groq.com/openai/v1/chat/completions"
    local headers = {
        ["Content-Type"] = "application/json",
        ["Authorization"] = "Bearer " .. api_key
    }
    
    local payload = {
        model = model,
        messages = {
            {
                role = "system",
                content = "You are a professional game translator. Translate the following text from Japanese/English to Thai. If the text is a name or proper noun, transliterate it to Thai. Return ONLY the translated text, no explanations, no quotes."
            },
            {
                role = "user",
                content = text
            }
        },
        temperature = 0.3
    }
    
    local body = nst_json_encode(payload)
    
    nst_log("[DEBUG] Sending request to Groq API...")
    local response_body, status = request_with_retry(url, "POST", headers, body)
    nst_log("[DEBUG] Received response - Status: " .. status)
    
    if status == 200 then
        local response = nst_json_decode(response_body)
        if response and response.choices and response.choices[1] and response.choices[1].message then
            local translated_text = response.choices[1].message.content
            nst_log("[DEBUG] Extracted translation: " .. translated_text)
            -- Trim whitespace
            translated_text = translated_text:gsub("^%s*(.-)%s*$", "%1")
            nst_log("[DEBUG] Returning translation: " .. translated_text)
            return translated_text, nil  -- Return 2 values: result and error (nil on success)
        else
            nst_log("[DEBUG] Failed to parse response structure")
        end
    end
    
    local error_msg = "Groq API Error: " .. status .. " - " .. (response_body or "nil")
    nst_log(error_msg)
    return nil, error_msg
end

function on_batch_text_extract(text_array)
    local api_key = nst_get_setting("api_key")
    local model = nst_get_setting("model")
    
    if not api_key or api_key == "" then
        local msg = "Error: Groq API Key not set in settings."
        nst_log(msg)
        return nil, msg
    end
    
    if not model or model == "" then model = MODEL end

    nst_log("Batch Translating " .. #text_array .. " items with Groq (" .. model .. ")...")

    local url = "https://api.groq.com/openai/v1/chat/completions"
    local headers = {
        ["Content-Type"] = "application/json",
        ["Authorization"] = "Bearer " .. api_key
    }
    
    -- Construct JSON array string for the prompt
    local json_texts = nst_json_encode(text_array)
    
    local payload = {
        model = model,
        messages = {
            {
                role = "system",
                content = "You are a professional game translator. Translate the following JSON array of texts from Japanese/English to Thai.\nIMPORTANT:\n- Output MUST be a valid JSON array of strings.\n- The length MUST match the input.\n- Transliterate names (e.g. 'Shiho' -> 'ชิโฮะ').\n- Do NOT return the original text. You MUST translate or transliterate every item.\n- Return ONLY the JSON array."
            },
            {
                role = "user",
                content = json_texts
            }
        },
        temperature = 0.3
    }
    
    local body = nst_json_encode(payload)
    
    local response_body, status = request_with_retry(url, "POST", headers, body)
    
    if status == 200 then
        nst_log("Groq Response: " .. response_body)
        local response = nst_json_decode(response_body)
        if response and response.choices and response.choices[1] and response.choices[1].message then
            local content = response.choices[1].message.content
            
            -- Find the JSON array (start with [ and end with ])
            local s, e = content:find("%[.*%]")
            if s then
                content = content:sub(s, e)
            else
                -- Try to clean up markdown if regex didn't match (e.g. multi-line)
                content = content:gsub("```json", ""):gsub("```", ""):gsub("^%s*", ""):gsub("%s*$", "")
            end
            
            local translated_array = nst_json_decode(content)
            if translated_array and type(translated_array) == "table" then
                return translated_array, nil  -- Return 2 values: result and error (nil on success)
            else
                nst_log("Error parsing batch response: " .. content)
                return nil, "Failed to parse batch response"
            end
        end
    end
    
    local error_msg = "Groq API Batch Error: " .. status .. " - " .. (response_body or "nil")
    nst_log(error_msg)
    return nil, error_msg
end
