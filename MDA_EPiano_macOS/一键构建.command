#!/bin/bash
cd "$(dirname "$0")" || exit 1
/bin/bash ./build_macos.sh
result=$?
if [[ $result -ne 0 ]]; then
  echo "构建失败。请把终端中的错误信息发回。"
fi
printf '\n按回车关闭此窗口。'
read -r ignored
exit "$result"
