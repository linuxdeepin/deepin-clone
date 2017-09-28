<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="zh_CN">
<context>
    <name>CloneJob</name>
    <message>
        <location filename="../src/corelib/clonejob.cpp" line="143"/>
        <source>Writing data to %1 failed, %2 byte data should be written, but actually %3 wrote, error: %4</source>
        <translation>写数据到 &quot;%1&quot; 失败，应写入%2字节数据，实际写入%3字节，错误信息：%4</translation>
    </message>
    <message>
        <source>%1 not exsit</source>
        <translation type="vanished">文件 &quot;%1&quot; 不存在</translation>
    </message>
    <message>
        <source>%1 invalid or not exsit</source>
        <translation type="vanished">&quot;%1&quot; 是一个不存在或者无效的文件</translation>
    </message>
    <message>
        <location filename="../src/corelib/clonejob.cpp" line="190"/>
        <source>%1 not exist</source>
        <translation>文件 &quot;%1&quot; 不存在</translation>
    </message>
    <message>
        <location filename="../src/corelib/clonejob.cpp" line="204"/>
        <location filename="../src/corelib/clonejob.cpp" line="231"/>
        <source>%1 invalid or not exist</source>
        <translation>&quot;%1&quot; 是一个不存在或者无效的文件</translation>
    </message>
    <message>
        <location filename="../src/corelib/clonejob.cpp" line="216"/>
        <source>Disk only can be cloned to disk</source>
        <translation>磁盘介质只能克隆到磁盘介质</translation>
    </message>
    <message>
        <location filename="../src/corelib/clonejob.cpp" line="237"/>
        <source>%1 total capacity is less than maximum readable data on %2</source>
        <translation>&quot;%1&quot; 的总容量比 &quot;%2&quot; 的最大可读数据小</translation>
    </message>
    <message>
        <location filename="../src/corelib/clonejob.cpp" line="251"/>
        <source>Failed to change %1 size, please check the free sapce on target disk</source>
        <translation>改变文件 &quot;%1&quot; 的大小失败，请检查目标磁盘可用空间</translation>
    </message>
</context>
<context>
    <name>MainWindow</name>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="224"/>
        <location filename="../src/widgets/mainwindow.cpp" line="556"/>
        <source>Cancel</source>
        <translation>取消</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="311"/>
        <source>Select Operation</source>
        <translation>请选择操作</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="312"/>
        <source>Next</source>
        <translation>下一步</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="333"/>
        <location filename="../src/widgets/mainwindow.cpp" line="743"/>
        <source>Backup</source>
        <translation>开始备份</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="335"/>
        <location filename="../src/widgets/mainwindow.cpp" line="745"/>
        <source>Clone</source>
        <translation>开始克隆</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="338"/>
        <location filename="../src/widgets/mainwindow.cpp" line="345"/>
        <source>Target disk will be permanently overwritten, please confirm to continue</source>
        <translation>克隆磁盘会删除目标磁盘的所有数据，请确认后再开始克隆</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="340"/>
        <location filename="../src/widgets/mainwindow.cpp" line="347"/>
        <source>Target partition will be permanently overwritten, please confirm to continue</source>
        <translation>克隆分区会删除目标分区的所有数据，请确认后再开始克隆</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="342"/>
        <location filename="../src/widgets/mainwindow.cpp" line="747"/>
        <source>Restore</source>
        <translation>开始还原</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="359"/>
        <source>Please move image file to other location outside the disk to avoid data loss</source>
        <translation>请移动镜像文件到恢复磁盘以外的位置，避免恢复磁盘时镜像文件被删除</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="361"/>
        <source>Please move image file to other location outside the partition to avoid data loss</source>
        <translation>请移动镜像文件到恢复分区以外的位置，避免恢复分区时镜像文件被删除</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="374"/>
        <source>Storage location can not be in the disk to backup, please reselect</source>
        <translation>存储位置不能在您要备份的磁盘内，请重新选择</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="376"/>
        <source>Storage location can not be in the partition to backup, please reselect</source>
        <translation>存储位置不能在您要备份的分区内，请重新选择</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="393"/>
        <source>Not enough total capacity in target disk, please select another one</source>
        <translation>目标磁盘总容量不足，请选择其他磁盘</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="395"/>
        <source>Not enough total capacity in target partition, please select another one</source>
        <translation>目标分区总容量不足，请选择其他分区</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="408"/>
        <source>Not enough total capacity, please select another disk</source>
        <translation>存储位置的总容量不足，请选择其他磁盘</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="437"/>
        <source>Proceed to clone?</source>
        <translation>您确定要继续吗？</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="438"/>
        <source>All data in target loacation will be formated during cloning or restoring disk (partition) without cancelable operation.</source>
        <translation>克隆或还原磁盘（分区）会格式化目标位置的所有数据，且过程中不可取消。</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="440"/>
        <source>Warning</source>
        <translation>警告</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="441"/>
        <location filename="../src/widgets/mainwindow.cpp" line="492"/>
        <location filename="../src/widgets/mainwindow.cpp" line="595"/>
        <location filename="../src/widgets/mainwindow.cpp" line="700"/>
        <source>OK</source>
        <translation>确定</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="457"/>
        <location filename="../src/widgets/mainwindow.cpp" line="538"/>
        <source>The selected storage location not found</source>
        <translation>未找到所选择的储存位置</translation>
    </message>
    <message>
        <source>%1 not exsit</source>
        <translation type="vanished">文件 &quot;%1&quot; 不存在</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="668"/>
        <source>Failed to restart system</source>
        <translation>重启系统失败</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="682"/>
        <source>Failed to restart &quot;Deepin Recovery&quot;</source>
        <translation>重启到“恢复模式”失败</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="487"/>
        <source>Restart to Continue</source>
        <translation>重启并继续</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="471"/>
        <location filename="../src/widgets/mainwindow.cpp" line="526"/>
        <source>%1 not exist</source>
        <translation>文件 &quot;%1&quot; 不存在</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="490"/>
        <source>&quot;%1&quot; is used, please restart and enter &quot;Deepin Recovery&quot; to continue</source>
        <translation>“%1”正在被使用，您可以重启进入“维护模式”继续操作</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="495"/>
        <source>&quot;%1&quot; is used, please install &quot;Deepin Recovery&quot; to retry</source>
        <translation>“%1”正在被使用，请安装“维护模式”后重试</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="550"/>
        <source>Performing Backup</source>
        <translation>正在备份</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="552"/>
        <source>Cloning</source>
        <translation>正在克隆</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="554"/>
        <source>Restoring</source>
        <translation>正在还原</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="570"/>
        <source>Backup Failed</source>
        <translation>备份失败</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="572"/>
        <source>Clone Failed</source>
        <translation>克隆失败</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="574"/>
        <source>Restore Failed</source>
        <translation>还原失败</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="579"/>
        <source>Retry</source>
        <translation>重新开始</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="582"/>
        <source>Task done</source>
        <translation>任务完成</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="585"/>
        <source>Backup Succeeded</source>
        <translation>备份成功</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="586"/>
        <source>View Backup File</source>
        <translation>查看备份文件</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="594"/>
        <source>Clone Succeeded</source>
        <translation>克隆成功</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="598"/>
        <source>Restore Succeeded</source>
        <translation>还原成功</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="599"/>
        <source>Restart</source>
        <translation>重新启动</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="759"/>
        <source>Disk</source>
        <translation>磁盘</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="761"/>
        <source>Partition</source>
        <translation>分区</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <location filename="../src/corelib/ddevicediskinfo.cpp" line="241"/>
        <source>process &quot;%1 %2&quot; crashed</source>
        <translation>进程 &quot;%1 %2&quot; 崩溃了</translation>
    </message>
    <message>
        <location filename="../src/corelib/ddevicediskinfo.cpp" line="243"/>
        <source>Failed to perform process &quot;%1 %2&quot;, error: %3</source>
        <translation>进程 &quot;%1 %2&quot; 执行失败，错误信息：%3</translation>
    </message>
    <message>
        <location filename="../src/corelib/ddevicediskinfo.cpp" line="250"/>
        <location filename="../src/corelib/ddevicediskinfo.cpp" line="265"/>
        <source>&quot;%1&quot; is not a disk device</source>
        <translation>&quot;%1&quot;不是一个 磁盘设备</translation>
    </message>
    <message>
        <location filename="../src/corelib/ddevicediskinfo.cpp" line="292"/>
        <source>&quot;%1&quot; is busy</source>
        <translation>&quot;%1&quot; 正忙</translation>
    </message>
    <message>
        <location filename="../src/corelib/ddevicediskinfo.cpp" line="319"/>
        <source>Failed to start &quot;%1 %2&quot;, error: %3</source>
        <translation>&quot;%1 %2&quot;启动失败，错误信息：%3</translation>
    </message>
    <message>
        <location filename="../src/corelib/ddevicediskinfo.cpp" line="330"/>
        <source>Failed to open process, error: %1</source>
        <translation>进程打开失败，错误信息：%1</translation>
    </message>
    <message>
        <location filename="../src/corelib/dfilediskinfo.cpp" line="178"/>
        <source>Failed to open file(%1), error: %2</source>
        <translation>打开文件&quot;%1&quot;失败，错误信息：%2</translation>
    </message>
    <message>
        <location filename="../src/corelib/helper.cpp" line="184"/>
        <source>%1 d %2 h %3 m</source>
        <translation>%1天%2小时%3分钟</translation>
    </message>
    <message>
        <location filename="../src/corelib/helper.cpp" line="187"/>
        <source>%1 h %2 m</source>
        <translation>%1小时%2分钟</translation>
    </message>
    <message>
        <location filename="../src/corelib/helper.cpp" line="190"/>
        <source>%1 m</source>
        <translation>%1分钟</translation>
    </message>
    <message>
        <location filename="../src/corelib/helper.cpp" line="192"/>
        <source>%1 s</source>
        <translation>%1秒</translation>
    </message>
    <message>
        <location filename="../src/main.cpp" line="96"/>
        <source>Deepin Clone</source>
        <translation>深度备份还原工具</translation>
    </message>
    <message>
        <location filename="../src/main.cpp" line="97"/>
        <source>Deepin Clone is a tool to backup and restore in deepin. It supports to clone, backup and restore disk or partition and other functions.</source>
        <translation>备份还原工具是深度操作系统中的备份还原小工具，包括磁盘克隆、磁盘备份、磁盘还原、分区克隆、分区备份、分区还原等功能。</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="72"/>
        <source>Partition &quot;%1&quot; not found</source>
        <translation>无法找到分区&quot;%1&quot;</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="74"/>
        <source>Disk &quot;%1&quot; not found</source>
        <translation>无法找到磁盘&quot;%1&quot;</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="99"/>
        <source>Failed to mount partition &quot;%1&quot;</source>
        <translation>分区&quot;%1&quot;挂载失败</translation>
    </message>
</context>
<context>
    <name>SelectActionPage</name>
    <message>
        <location filename="../src/widgets/selectactionpage.cpp" line="75"/>
        <source>Select media</source>
        <translation>请选择操作介质</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectactionpage.cpp" line="76"/>
        <source>Select operation for media</source>
        <translation>请选择介质对应的操作</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectactionpage.cpp" line="85"/>
        <source>Disk</source>
        <translation>磁盘</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectactionpage.cpp" line="94"/>
        <source>Partition</source>
        <translation>分区</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectactionpage.cpp" line="119"/>
        <source>Clone Disk</source>
        <translation>克隆磁盘</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectactionpage.cpp" line="119"/>
        <source>Clone source disk to target disk</source>
        <translation>克隆磁盘数据到另一块磁盘</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectactionpage.cpp" line="122"/>
        <source>Disk to Image</source>
        <translation>备份磁盘到镜像</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectactionpage.cpp" line="122"/>
        <source>Backup disk data to a image file</source>
        <translation>备份磁盘数据到一个镜像文件</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectactionpage.cpp" line="125"/>
        <source>Image to Disk</source>
        <translation>从镜像恢复到磁盘</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectactionpage.cpp" line="125"/>
        <source>Restore image file to disk</source>
        <translation>从镜像文件还原数据到磁盘</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectactionpage.cpp" line="131"/>
        <source>Clone Partition</source>
        <translation>克隆分区</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectactionpage.cpp" line="131"/>
        <source>Clone source partition to target partition</source>
        <translation>克隆分区数据到另一个分区</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectactionpage.cpp" line="134"/>
        <source>Partition to Image</source>
        <translation>备份分区到镜像</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectactionpage.cpp" line="134"/>
        <source>Backup partition data to a image file</source>
        <translation>备份分区数据到一个镜像文件</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectactionpage.cpp" line="137"/>
        <source>Image to Partition</source>
        <translation>从镜像恢复到分区</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectactionpage.cpp" line="137"/>
        <source>Restore image file to partition</source>
        <translation>从镜像文件还原数据到分区</translation>
    </message>
</context>
<context>
    <name>SelectFilePage</name>
    <message>
        <location filename="../src/widgets/selectfilepage.cpp" line="234"/>
        <source>Select the source disk</source>
        <translation>请选择您要克隆的原始磁盘</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectfilepage.cpp" line="235"/>
        <source>Select the target disk</source>
        <translation>请选择被克隆的目标磁盘</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectfilepage.cpp" line="249"/>
        <source>Select the source partition</source>
        <translation>请选择您要克隆的原始分区</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectfilepage.cpp" line="250"/>
        <source>Select the target partition</source>
        <translation>请选择被克隆的目标分区</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectfilepage.cpp" line="278"/>
        <source>Select a disk to backup</source>
        <translation>请确认您要备份的磁盘</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectfilepage.cpp" line="286"/>
        <source>Select a partition to backup</source>
        <translation>请确认您要备份的分区</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectfilepage.cpp" line="295"/>
        <source>Select storage location</source>
        <translation>请选择存储位置</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectfilepage.cpp" line="321"/>
        <source>Select a backup image file</source>
        <translation>请选择备份的镜像文件</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectfilepage.cpp" line="324"/>
        <source>Select a disk to restore</source>
        <translation>请选择需要恢复的磁盘</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectfilepage.cpp" line="326"/>
        <source>Select a partition to restore</source>
        <translation>请选择需要恢复的分区</translation>
    </message>
</context>
<context>
    <name>SelectFileWidget</name>
    <message>
        <location filename="../src/widgets/selectfilepage.cpp" line="93"/>
        <source>Select storage location</source>
        <translation>请选择存储位置</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectfilepage.cpp" line="93"/>
        <source>Select image file</source>
        <translation>选择镜像文件</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectfilepage.cpp" line="99"/>
        <source>Drag and drop backup image file here</source>
        <translation>拖拽备份的镜像文件到此</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectfilepage.cpp" line="120"/>
        <source>Deepin Image File</source>
        <translation>深度系统镜像文件</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectfilepage.cpp" line="186"/>
        <source>Reselect image file</source>
        <translation>重新选择镜像文件</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectfilepage.cpp" line="189"/>
        <source>Reselect storage location</source>
        <translation>重新选择储存位置</translation>
    </message>
</context>
<context>
    <name>WorkingPage</name>
    <message>
        <location filename="../src/widgets/workingpage.cpp" line="44"/>
        <source>Task is ongoing, please wait......</source>
        <translation>任务正在进行，请稍候...</translation>
    </message>
    <message>
        <location filename="../src/widgets/workingpage.cpp" line="61"/>
        <source>Progress: %1/%2</source>
        <translation>进度：%1/%2</translation>
    </message>
    <message>
        <location filename="../src/widgets/workingpage.cpp" line="62"/>
        <source>Remaining time: %1</source>
        <translation>预计剩余时间：%1</translation>
    </message>
</context>
</TS>
