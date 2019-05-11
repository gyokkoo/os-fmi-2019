## Подготовка за Контролно 2

**SI-2016-z1** <br/>Напишете shell скрипт, който по подаден един позиционен параметър, ако този параметър е директория, намира всички symlink-ове в нея и под-директориите ѝ с несъществуващ destination.<br/>

Решение:

``````bash
#!/bin/bash

if [[ $# -ne 1 || ! -d $1 ]]; then
    exit 1
fi

# Option 1
find -L $1 -type l

# Option 2
for i in $(find $1 -type l); do
    dst=$(stat -c %N $i | cut -d "'" -f 4)
    if [ ! -e $(dirname $i)/$dst ]; then
        echo $i
    fi
done

# Option 3
find $1 -type l | xargs ls -l | while read line; do
    src=$(echo $line | tr -s ' ' | cut -d ' ' -f 9)
    dst=$(echo $line | tr -s ' ' | cut -d ' ' -f 11)
    if [ ! -e $(dirname $src)/$dst ]; then
        echo $src
    fi
done
``````

**SI-2016-z2**<br/>Напишете shell скрипт, който приема един позиционен параметър - число. Ако скриптът се изпълнява като root, да извежда обобщена информация за общото количество активна памет (RSS - resident set size, non-swapped physical memory that a task has used) на процесите на всеки потребител. Ако за някой потребител обобщеното число надвишава подадения параметър, да изпраща подходящи сигнали за прекратяване на процеса с най-много активна памет на потребителя.

Забележка: Приемаме, че изхода в колоната RSS е число в същата мерна единица, като числото, подадено като аргумент. Примерен формат:

```
USER       PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
root         1  0.0  0.0  15816  1884 ?        Ss   May12   0:03 init [2]
root         2  0.0  0.0      0     0 ?        S    May12   0:00 [kthreadd]
root         3  0.0  0.0      0     0 ?        S    May12   0:02 [ksoftirqd/0]
```

Алтернативно може да ползвате изхода от ps -e -o uid,pid,rss

Решение (авторско):

``````bash
#!/bin/bash

if [[ $# -ne 1 || $(id -u) -ne 0 ]]; then
    exit 1
fi

users="$(ps -e -o user | tail -n +2 | sort | uniq )"
for user in $users; do
    total_rss=0
    while read line; do
        current_pid=$(echo $line | awk '{print $1}')
        current_rss=$(echo $line | awk '{print $2}')
        total_rss=$(expr $total_rss + $current_rss)
    done < <(ps -u $user -o pid,rss | tail -n +2 | sort -n -k 2)

    echo "user:" $user "total_rss:" $total_rss

    if [[ $total_rss -gt $1 ]]; then
        echo "Greater than $1, will kill $current_pid"
        kill -s SIGTERM $current_pid
        sleep 2
        kill -s SIGKILL $current_pid
    fi
done
``````

**SI-2016-z3**<br/>Напишете shell скрипт който, ако се изпълнява от root, проверява кои потребители на системата нямат homedir или не могат да пишат в него.

Примерен формат:

```
root:x:0:0:root:/root:/bin/bash
daemon:x:1:1:daemon:/usr/sbin:/usr/sbin/nologin
bin:x:2:2:bin:/bin:/usr/sbin/nologin
```

Решение:

``````bash
#!/bin/bash

if [[ $(id -u) -ne 0 ]]; then
    exit 1
fi

while read user_entry; do
    username=$(echo $user_entry | cut -d ':' -f 1)
    uid=$(echo $user_entry | cut -d ':' -f 3)
    gid=$(echo $user_entry | cut -d ':' -f 4)
    home=$(echo $user_entry | cut -d ':' -f 6)

    if [[ -z $home ]]; then
        echo "$username has no homedir set"
        continue
    fi

    if [[ ! -d $home ]]; then
        echo "$username has homedir $home but is not a directory"
        continue
    fi

    dir_permissions=$(ls -ld $home | awk '{print $1}')
    dir_owner=$(ls -ld $home | awk '{print $3}')

    if [[ $dir_owner != $username ]]; then
        echo "$username is not owner of $home"
        continue
    fi

    if [[ $(echo $dir_permissions | cut -c 3) != "w" ]]; then
        echo "$username (owner) cannot write in $home"
    fi
done < <(cat /etc/passwd)
``````

**SI-2016-z4**   (Теоретична)

Опишете накратко основните <u>структури данни</u> и <u>процедури</u>, необходими за реализиране на семафор. <br/>

``````
Структурите данни, необходими за реализация на семафор са:
- Брояч, в който се пази броя на процесите, които могат да бъдат допуснати до ресурса, охраняван от семафора.
- Контейнер Q, в който се пази информация кои процеси чакат достъп до ресурса

Процедури необходими за реализация на семафор са:
- Конструктор Init(cnt), който задава начална стойност на брояча и контейнерът се инициализира празен
- Метод Wait() - намаля брояча, когато се достъпва ресурса и ако броячът стане отрицателен, процесът викащ Wait се блокира и номерът на процеса се вкарва в контейнера.
- Метод Signal() - увеличава брояча, когато завършва работа с процеса и вади процес следващият процес от контейнера и го активира.
``````

Каква е разликата между слаб и силен семафор?

``````
Силен семафор - когато контейнерът е реализиран като обикновена опашка
Слаб семафор - когато контейнерът не е опашка и се активира процес който не е първи в опашката
``````

<hr />

**SI-2017-z1**<br/>Напишете скрипт, който получава задължителен първи позиционен параметър - директория и незадължителен втори - число. Скриптът трябва да проверява подадената директория и нейните под-директории и да извежда имената на:

- при подаден на скрипта втори параметър - всички файлове с брой hardlink-ове поне равен на параметъра
- при липса на втори параметър - всички symlink-ове с несъществуващ destination (счупени simlink-ове)

Заб.: За удобство приемаме, че ако има подаден втори параметър, то той е число.

``````bash
#!/bin/bash

check_dir() {
    if [[ ! -d "$1" ]]; then
        echo "$1 is a directory!"
        exit 2
    fi
}

print_usage() {
    echo "usage: $0 <dirname> [<number>]"
}

case $# in
    1)
        check_dir "$1"
        find -L -type l
        ;;
    2)
        check_dir "$1"
        for i in $(find "$1" -type f); do
            if [[ $(stat -c '%h' "${i}") -ge $2 ]]; then
                echo "${i}"
            fi
        done
        ;;
    *)
        print_usage
        exit 1
esac
``````

**SI-2017-z2**<br/> Напишете скрипт, който приема три задължителни позиционни параметра - директория SRC, директория DST (която не трябва да съдържа файлове) и низ АBC. Ако скриптът се
изпълнява от root потребителя, то той трябва да намира всички файлове в директорията SRC и
нейните под-директории, които имат в името си като под-низ АBC, и да ги мести в директорията
DST, запазвайки директорийната структура (но без да запазва мета-данни като собственик и права,
т.е. не ни интересуват тези параметри на новите директории, които скриптът би генерирал в DST).

``````bash
#!/bin/bash

check_dir() {
    if [[ ! -d "$1" ]]; then
        echo "$1 not a dir"
        exit 3
    fi
}

if [[ $# -ne 3 ]]; then
    echo "usage: $0 <dirname> <dirname> <string>"
    exit 1
fi

if [[ $(id -u) -ne 0 ]]; then
    echo "Must be run as root"
    exit 2
fi

check_dir "$1"
check_dir "$2"

if [[ $(find "$2" -type f | wc -l) -ne 0 ]]; then
    echo "$2 contains files"
    exit 4
fi

SRC=$(dirname "$1")"/"$(basename "$1")
DST=$(dirname "$2")"/"$(basename "$2")
ABC="$3"

for i in $(find "${SRC}" -type f -name "*${ABC}*"); do
    DSTFILE=$(echo ${i} | sed -e "s|${SRC}}${DST}")
    mkdir -p $(dirname $DSTFILE)
    mv ${i} ${DSTFILE}
done	
``````

**SI-2017-z3** <br/>Напишете скрипт, който ако се изпълнява от root потребителя:

* извежда обобщена информация за броя и общото количество активна памет (RSS - resident set size, non-swaped physical memory that a task has used) на текущите процеси на всеки потребител 

* ако процесът с най-голяма активна памет на даден потребител използва два пъти повече памет от средното за потребителя, то скриптът да прекратява изпълнението му по подходящ начин.

  За справка:

``````bash
#!/bin/bash

if [[ $(id -u) -ne 0 ]]; then
    echo "Must be ran as root"
    exit 1
fi

for U in $(ps -e -o user | tail -n +2 | sort | uniq ); do
    TOTAL_RSS=0
    PS_COUNT=0

    while read CPID CRSS; do
        PS_COUNT=$(expr ${PS_COUNT} + 1)
        TOTAL_RSS=$(expr ${TOTAL_RSS} + ${CRSS})
        MAX_RSS=${CRSS}
        MAX_RSS_PID=${CPID}
    done < <(ps -u ${U} -o pid,rss | tail -n +2 | sort -n -k 2)

    if [[ ${PS_COUNT} -gt 0 ]]; then
        AVGRSS=$(expr ${TOTAL_RSS} / ${PS_COUNT})
        echo "${U} ${PS_COUNT} ${TOTAL_RSS}"

        if [[ ${MAX_RSS} -gt $(expr ${AVGRSS} "*" 2) ]]; then
            echo -e "\tpid ${MAX_RSS_PID} has ${MAX_RSS}, will terminate"
            kill -s SIGTERM ${MAX_RSS_PID}
            sleep 2
            kill -s SIGKILL ${MAX_RSS_PID}
        fi
    fi
done	
``````

**SI-2017-z4**<br/>Напишете shell script, който получава задължителен първи позиционен параметър - директория и незадължителен втори - име на файл. Скриптът трябва да намира в подадената деиректория:

- броя на symlink-овете, чийто destination не съществува
- Името на симлинка "->" името на destination, ако съществува

Ако е подаден като аргумент файл - да се добавят в него, а ако не е - да се извеждат на екрана.

``````
	
``````

**SI-2017-z5**<br/>Напишете скрипт, който приема задължителен позиционен аргумент - име на потребител (FOO). Ако скриптът се изпълнява от root:

a) да извежда имената на потребителите, които имат повече на брой процеси от FOO, ако има такива;

б) да извежда средното време (в секунди), за което са работили процесите на всички потребители на системата (TIME);

в) ако съществуват процеси на FOO, които са работили над два пъти от средното време, скриптът да прекратява изпълнението им по подходящ начин

``````

``````

**SI-2017-z6**<br/>Напишете скрипт, който извежда името на потребителския акаунт, в чиято home директория има най-скоро променен обикновен файл и кой е този файл. Напишете скрипта с подходящите проверки, така че да бъде валиден инструмент.

``````bash
#!/bin/bash

declare -r user_id=$(id -u)
if [ "${user_id}" -ne 0 ]; then
    echo "Error! This script must be executed as root!" >&2
    exit 1
fi

result=$(while read line; do
    username=$(echo $line | cut -d ':' -f 1)
    home_dir=$(echo $line | cut -d ':' -f 6)

    if [ ! -d "${home_dir}" ]; then
        continue;
    fi

    find "${home_dir}" -type f -printf "${username} %T@ %p \n"
done < <(cat /etc/passwd))

echo "${result}" | sort -k 2 -rn | head -n 1 | cut -d ' ' -f 1,3
exit 0
``````



**SI-2017-z7**<br/>Нека съществува програма за моментна комуникация (Instant messaging), която записва логове на разговорите в следния формат:

- има определена директория за логове (LOGDIR)
- в нея има директорийна структура от следния вид:
  		LOGDIR/протокол/акаунт/приятел/
    	като на всяко ниво може да има няколко екземпляра от съответния вид, т.е. няколко директории *протокол*, във всяка от тях може да има няколко директории *акаунт*, и във всяка от тях -- няколко директории *приятел*.

- във всяка от директориите *приятел* може да има файлове с имена от вида yyyy-mm-dd-hh-mm-ss.txt (година-месец-ден и т.н., спрямо това кога е започнал даден разговор
- всеки такъв файл представлява лог на даден разговор със съответния приятел, като всяка разменена реплика между вас е на отделен ред
- даден идентификатор *приятел* може да се среща няколко пъти в структурата (напр. през различни ваши акаунти сте водили разговори със същия приятел)

Напишете скрипт, който приема задължителен позиционен аргумент - име на лог директория (LOGDIR). Скриптът трябва да извежда десетимата приятели, с които имате най-много редове комуникация глобално (без значение протокол и акаунт), и колко реда имате с всеки от тях.

``````bash
#!/bin/bash
[ $# -ne 1 ] && echo "Usage: $0 datadir" 1>&2 && exit 1
data="$1"
fpat=’????-??-??-??-??-??.txt’
[ -d "${data}" ] || exit 2
declare -A arr
for abuddy in $(find -- "${data}" -mindepth 3 -maxdepth 3 -type d); do
        buddy="$(basename -- "${abuddy}")"
        lines=$(find -- ${abuddy} -type f -name ${fpat} | xargs cat | wc -l)
        arr[${buddy}]=$(( ${arr[${buddy}]:-0} + ${lines} ))
done
for buddy in "${!arr[@]}"; do
        echo ${arr[${buddy}]} ${buddy}
done | sort -rn | head
или
#!/bin/bash
[ $# -ne 1 ] && echo "Usage: $0 datadir" 1>&2 && exit 1
data="$1"
fpat=’????-??-??-??-??-??.txt’
[ -d "${data}" ] || exit 2
for fname in $(find -- "${data}" -mindepth 4 -maxdepth 4 -type f -name ${fpat}); do
        buddy="$(basename -- "$(dirname -- "${fname}")")"
        lines=$(wc -l < "${fname}")
        echo "${buddy} ${lines}"
done \
        | awk ’{ arr[$1] += $2 } END { for (key in arr) printf("%s %s\n", arr[key], key) }’ \
        | sort -rn | head

# Option 2
#!/bin/bash
[ $# -ne 1 ] && echo "Usage: $0 datadir" 1>&2 && exit 1
data="$1"
fpat=’????-??-??-??-??-??.txt’
[ -d "${data}" ] || exit 2
(
  tbuddy=""
  sum=0
  while read buddy lines; do
    if [ "${buddy}" != "${tbuddy}" ]; then
      [ -z ${tbuddy} ] || echo ${sum} ${tbuddy}
      tbuddy="${buddy}"
      sum=${lines}
    else
      sum=$(( ${sum} + ${lines} ))
fi done
  [ -z ${tbuddy} ] || echo ${sum} ${tbuddy}
) < <(
  for fname in $(find -- "$data/" -mindepth 4 -maxdepth 4 -type f -name $fpat); do
    buddy="$(basename -- "$(dirname -- "$fname")")"
    lines=$(wc -l < "${fname}")
    echo "${buddy} ${lines}"
  done | sort
) | sort -rn | head

# Option 3
#!/bin/bash
[ $# -ne 1 ] && echo "Usage: $0 datadir" 1>&2 && exit 1
data="$(dirname -- ${1})/$(basename ${1})"
fpat=’????-??-??-??-??-??.txt’
[ -d "${data}" ] || exit 2
for buddy in $(find -- "${data}" -mindepth 3 -maxdepth 3 -type d \
    -exec basename {} \; | sort | uniq ); do
  echo "$(cat ${data}/*/*/${buddy}/${fpat} | wc -l) ${buddy}"
done | sort -rn | head
``````

**SI-2016-Sep-z1** <br/>

``````bash
#!/bin/bash

if [[ $(id -u) -ne 0 ]]; then
    echo "You are not root"
    exit 1
fi

cut -d ':' -f 7 /etc/passwd | sort | uniq -c | sort -nr | head -n 3
``````

**SI-2016-Sep-z2** <br/>

``````bash
#!/bin/bash

if [[ $# -ne 2 || $(id -u) -ne 0 ]]; then
    echo "Usage: $0 <username> <number>"
    echo "Should br run as root"
    exit 1
fi

ps -u $1 -o pid= | while read PID; do
    if [[ $PID -ge $2 ]]; then
        echo "Greater than $2, will kill $PID"
        kill -s SIGTERM $PID
        sleep 2
        kill -s SIGKILL $PID
    fi
done
``````



**SI-2017-Sep-z1** <br/>Напишете скрипт, който получава два задължителни позиционни параметъра – директория и низ. Сред файловете в директорията би могло да има такива, чиито имена спазват следната
структура: vmlinuz-x.y.z-arch, където

- vmlinuz е константен низ;

- тиретата “-” и точките “.” присъстват задължително;

- x е число, version;

- y е число, major revision;

- z е число, minor revision;

- наредената тройка x.y.z формира глобалната версия на ядрото;

- arch е низ, архитектура (платформа) за която е съответното ядро.

  

Скриптът трябва да извежда само името на файла, намиращ се в подадената директория (но не и нейните поддиректории), който: 

-  спазва гореописаната структура;
- е от съответната архитектура спрямо параметъра-низ, подаден на скрипта;
- има най-голяма глобална версия.

``````

``````



**SI-2018-z2** <br/>Напишете скрипт, който приема три задължителни позицонни аргумента:

- име на фаил
- низ1
- низ2

Файлът е текстови, и съдържа редове във формат:  ключ =стойност, където стойност може да бъде:

- Празен низ, т.е. редът е ключ
- низ, състоящ се от един или повече термове, разделени с интервали, т.е. редът е ключ=t1 t2 t3

Някъде във файла:

- се съдържа един ред с ключ първия подаден низ (низ1);
- и може да се съдържа един ред с ключ втория подаден низ (низ2). 

Скриптът трябва да променя реда с ключ низ2 така, че обединението на термовете на редовете с ключове низ1 и низ2 да включва всеки терм еднократно. 

``````bash
#!/bin/bash

if [ $# -ne 3  ]; then
   echo "Error, must have three arguments, but it has ${#}: ${@}"
   exit 1
fi

S1=$(egrep "^$2=" $1 | cut -d '=' -f 2)

matches_count=$(egrep -c "^$3=" $1)
if [ ${matches_count} -eq 0 ]; then
    echo "$3" >> $1
    exit 0
fi

S2=$(egrep "^$3=" $1 | cut -d '=' -f 2)
NEWS2=$(comm -13 <(echo "$1" | tr -s ' ' | tr -s ' ' '\n' | sort) <(echo $S2 | tr -s ' ' | tr ' ' '\n' | sort ) | xargs )
sed -i -e "s/^$3=$S2/$3=$NEWS2/" $1
``````

#### More tasks & solutions from tasks_8.txt file

**Task 10**<br/>Iterates over the numbers from 1 to 100 and for each number *kratno
na* 3 prints 'FOO', for each number *kratno na* 5 prints 'BAR'.

``````bash
#!/bin/bash

result=''
for i in $(seq 1 100); do
    if [[ $(($i % 3)) -eq 0 ]]; then
        result="FOO"
    fi

    if [[ $(($i % 5)) -eq 0 ]]; then
        result="${result}BAR"
    fi

    # The -n test requires that the string be quoted within the test brackets.
    # Using an unquoted string with ! -z, or even just the unquoted string alone within test brackets
    # normally works, however, this is an unsafe practice.
    # Always quote a tested string
    # -n test string is not null
    if [[ -n "$result" ]]; then
        echo $result
    fi
    result=''
done
``````

**Task 12**<br/>For each regular file or directory in the '/tmp' directory print:
-) If it is a directory - 'This is a directory - \<directory name>'
-) If it is a file - 'The file \<filename> is in \<directory name>'
Example:

If the '/tmp' directory contains:
/tmp/dir1/regular_file
/tmp/dir2
/tmp/symlink
The expected output is:
This is a directory - dir2
The file regular_file is in /tmp/dir2

``````bash
#!/bin/bash

src=$1
if [[ -z "$src" ]]; then
    src="/tmp"
fi

echo "Traversing ${src}"
find "$src" 2> /dev/null | while read name; do
    if [ -f "$name" ]; then
        echo "The file $(basename $name) is in $(dirname $name)."
    fi

    if [ -d "$name" ]; then
        echo "This is a directory - $(basename $name)."
    fi
done
``````

**Task 13** <br/>You have the following setup:
/home/trenchev/example_data/files_creation.log
/home/trenchev/example_data/files_directory

'files_creation.log' contains rows which describe an action with a file. Each row has 3 fields separated by a white space: <Epoch time timestamp> <Action> <File name> Where <Action> is either 'del' or 'add'.

The log file describes the state of the files in 'files_directory'. Meaning for each 'add' action (which was not later 'del'eted) there should be a file with that name. For example, if the log contains:

1 add pesho

11 add gosho

15 add ivan

18 del pesho

25 del gosho

41 add gosho

The 'files_directory' should contain the following files:

gosho

ivan

Your task is to write a script, which goes through the log file, determines the state in which 'files_directory' should be and reports and differences. For example, if according to the log file there should be a file called 'foo' in the 'files_firectory' but it is missing, a warning should be issued. For example: 'WARNING! According to log file foo should exist, but it is missing!' If there are any files in the directory which should not exist according to the log file, another warning should be issued. 

For example:
'WARNING! File foo found in files_directory, but there is no information about it in the log file'.

Note: The warning messages are just an example. Could be changed.

Solution:

````bash
#!/bin/bash

log_file='/home/trenchev/example_data/files_creation.log'
files_dir='/home/trenchev/example_data/files_directory'

if [[ ! -r "$log_file" ]]; then
    echo "$log_file could not be read. Aborting ..."
    exit 1
fi

if [[ ! -d "$files_dir" ]] || [[ ! -r "$files_dir" ]] || [[ ! -x "$files_dir" ]]; then
    echo "Could not read the content of $files_dir. Aborting..."
    exit 2
fi

log_file_description="/tmp/description_log_$(whoami)"
dir_files_description="/tmp/description_dir_$(whoami)"

cat "$log_file" | while read timestamp action name; do
    case "$action" in
        add)
            # Add the $name to what we expect to find in $files_dir
            echo "$name" >> "$log_file_description"
        ;;
        del)
           # Remove the $name from $log_file_description
           sed -i "/$name/d" "$log_file_description"
        ;;
        *)
           echo "WARNING! Unrecognized action '$action'. Skipping..."
        ;;
    esac
done

ls "$files_dir" > "$dir_files_description"
cat "$log_file_description" | while read name; do
    if ( ! grep -q "$name" "$dir_files_description" ); then
        echo "Warning! $name should exist, but is missing!"
    fi
done

cat "$dir_files_description" | while read name; do
    if ( ! grep -q "$log_file_description" ); then
        echo "Warning! File $name found in $files_dir, but there is no information about it in $log_file"
    fi
done

rm "$log_file_description" "$dir_files_description"
exit 0
````