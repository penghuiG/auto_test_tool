

import logging
import time
import os
log_path = "~/auto_test_test/test_p/log/"

# logger = logging.getLogger('tsu_test')
def log_file_name():
    current_time = time.localtime()
    formatted_time = time.strftime("%Y-%m-%d-%H:%M:%S", current_time)
    return '~/auto_test_test/test_p/log/tsu_test_' + formatted_time + ".log"
# 记录日志
# logger.debug('这是一个 debug 级别的日志（仅写入文件）')
# logger.info('这是一个 info 级别的日志（写入文件和控制台）')
# logger.warning('这是一个 warning 级别的日志（写入文件和控制台）')
# logger.error('这是一个 error 级别的日志（写入文件和控制台）')
# logger.critical('这是一个 critical 级别的日志（写入文件和控制台）')

def keep_latest_files(folder_path, num_files_to_keep=19):
    # 获取文件夹中的所有文件和子文件夹
    all_items = os.listdir(folder_path)
    
    # 过滤出文件（排除子文件夹）
    files = [item for item in all_items if os.path.isfile(os.path.join(folder_path, item))]
    
    if len(files) <= num_files_to_keep:
        print(f"文件夹中的文件数量 ({len(files)}) 小于或等于需要保留的数量 ({num_files_to_keep})，无需操作。")
        return

    # 获取每个文件的最后修改时间，并与文件名组合成元组
    file_time_pairs = []
    for file in files:
        file_path = os.path.join(folder_path, file)
        # 使用文件的最后修改时间（getmtime），也可以改为 getctime() 获取创建时间
        modification_time = os.path.getmtime(file_path)
        file_time_pairs.append((modification_time, file))
    
    # 按修改时间从大到小排序（最新的文件在前）
    file_time_pairs.sort(reverse=True, key=lambda x: x[0])
    
    # 取出最新的 num_files_to_keep 个文件
    latest_files = [file for _, file in file_time_pairs[:num_files_to_keep]]
    
    # 其余的文件是需要删除的
    files_to_delete = [file for _, file in file_time_pairs[num_files_to_keep:]]
    
    # 打印结果（可选）
    print(f"最新的 {num_files_to_keep} 个文件：")
    for file in latest_files:
        print(file)
    
    print(f"需要删除的文件：")
    for file in files_to_delete:
        print(file)
    
    # 如果需要删除文件，取消下面的注释
    for file in files_to_delete:
        file_path = os.path.join(folder_path, file)
        os.remove(file_path)
        print(f"已删除文件: {file}")
    
class log_t:
    def __init__(self):
        # 创建一个日志记录器
        self.logger = logging.getLogger('tsu_test')
        self.logger.setLevel(logging.DEBUG)  # 设置日志级别
        
        # 创建一个文件处理器，用于将日志写入文件
        keep_latest_files(folder_path = log_path)
        file_handler = logging.FileHandler(log_file_name())
        file_handler.setLevel(logging.DEBUG)  # 设置文件处理器的日志级别
        file_formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
        file_handler.setFormatter(file_formatter)

        # 创建一个控制台处理器，用于将日志输出到控制台
        console_handler = logging.StreamHandler()
        console_handler.setLevel(logging.INFO)  # 设置控制台处理器的日志级别
        console_formatter = logging.Formatter('%(name)s - %(levelname)s - %(message)s')
        console_handler.setFormatter(console_formatter)

        # 将处理器添加到日志记录器
        self.logger.addHandler(file_handler)
        self.logger.addHandler(console_handler)

    def debug(self,str):
        self.logger.debug(str)
    def info(self,str):
        self.logger.info(str)
    def warning(self,str):
        self.logger.warning(str)
    def error(self,str):
        self.logger.error(str)

my_log = log_t()
