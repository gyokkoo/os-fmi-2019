## Процеси

**Зад 1. 04-а-5000** <br/>Намерете командите на 10-те най-стари процеси в системата.

`ps -eo cmd | tail -n +2 | head`

**Зад 2. 04-a-6000**<br/>Намерете PID и командата на процеса, който заема най-много виртуална памет в системата.

`ps aux | sort -r -k 4 | tail -n +2 | head -n 1 | awk '{print $2, $11}'` или

`ps -eo vsz,pid,cmd | sort -r -k 1 | head -n 1 | awk '{print $2, $3}'`

**Зад 3. 04-b-5000**<br/>Намерете колко физическа памет заемат всички процеси на потребителската група SI.

`ps -eo pid,group,sz | grep SI | awk '{ sum += $3 } END  {print sum}'`

**Зад 4. 04-a-7000**<br/>Намерете колко физическа памет заемат осреднено всички процеси на потребителската група SI. Внимавайте, когато групата няма нито един процес.

`ps -eo pid,group,sz | grep SI | awk ' { sum += $3 } END  { if (NR==0) exit 1; else  print sum/NR}'`

**Зад 5. 04-b-8000**<br/>Намерете всички PID и техните команди (без аргументите), които нямат tty, което ги управлява. Изведете списък само с командите без повторения.

`ps -eo pid,tty,cmd | grep -v "tty" | awk '{ print $1, $3}'`

`ps -eo pid,tty,cmd | grep -v "tty" | awk '{ print $3}' | sort | uniq`

**Зад 6. 04-b-9000**<br/>Да се отпечатат PID на всички процеси, които имат повече деца от родителския си процес.

`ps -eo pid,ppid | awk '$1 > $2 { print $1, $2  }'`

``````bash
#!/bin/bash

children () {
    ps -e -o pid --ppid "${1}" --no-headers | wc -l
}

ps -e -o pid,ppid --no-headers | while read PID_VAR PPID_VAR; do
    if [[ ${PPID_VAR} -eq 0 ]]; then
        continue
    fi

    if [ $(children "${PID_VAR}") -gt $(children "${PPID_VAR}") ]; then
        echo "pid: ${PID_VAR}"
    fi
done
``````