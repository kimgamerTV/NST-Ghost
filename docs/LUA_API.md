# NST Lua Plugin API

## Overview
NST รองรับ Lua plugins เพื่อให้คนนอกขยายความสามารถได้

## Core Hooks

### `on_install()`
เรียกครั้งแรกเพื่อติดตั้งสิ่งจำเป็น
```lua
function on_install()
    nst_log("Installing dependencies...")
    nst_install_package("pip install unrpyc")
    return true
end
```

### `get_menu_items()`
คืนค่าชื่อเมนูที่จะแสดงใน menubar
```lua
function get_menu_items()
    return "My Plugin Action"
end
```

### `on_menu_click()`
เรียกเมื่อคลิกเมนู
```lua
function on_menu_click()
    nst_log("Menu clicked!")
end
```

## API Functions

### `nst_log(message)`
แสดง log
```lua
nst_log("Hello!")
```

### `nst_install_package(command)`
รันคำสั่ง shell เพื่อติดตั้ง
```lua
nst_install_package("pip install requests")
```

### `nst_translate(text)`
แปลข้อความ
```lua
local result = nst_translate("Hello")
```

## Examples
- `scripts/text_processor.lua` - Mock data
- `scripts/install_unrpyc.lua` - Install dependencies
