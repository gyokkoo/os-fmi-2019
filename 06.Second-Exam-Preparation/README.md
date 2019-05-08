## Подготовка за Контролно 2

**SI-2016-z1** <br/>Напишете shell скрипт, който по подаден един позиционен параметър, ако този параметър е директория, намира всички symlink-ове в нея и под-директориите ѝ с несъществуващ destination.<br/>

Решение (авторско):

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

###More tasks & solutions from tasks_8.txt file

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

'files_creation.log' contains rows which describe an action with a
file. Each row has 3 fields separated by a white space:
<Epoch time timestamp> <Action> <File name>
Where <Action> is either 'del' or 'add'.

The log file describes the state of the files in 'files_directory'.
Meaning for each 'add' action (which was not later 'del'eted) there
should be a file with that name. For example, if the log contains:
1 add pesho
11 add gosho
15 add ivan
18 del pesho
25 del gosho
41 add gosho
The 'files_directory' should contain the following files:
gosho
ivan

Your task is to write a script, which goes through the log file,
determines the state in which 'files_directory' should be and
reports and differences. For example, if according to the log file
there should be a file called 'foo' in the 'files_firectory' but it
is missing, a warning should be issued. For example:
'WARNING! According to log file foo should exist, but it is missing!'
If there are any files in the directory which should not exist
according to the log file, another warning should be issued. For
example:
'WARNING! File foo found in files_directory, but there is no
information about it in the log file'.
Note: The warning messages are just an example. Could be changed.

Solution:

````
// TODO: Solve when connected to os-server
````