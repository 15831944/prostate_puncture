# prostate_puncture
main program of prostate puncture

# v1.0
以 Calibrate 作为单启动项目，实现标定辅助的信息采集。0.3s的时间间隔，记录360个时刻，所有探头的位置以及相应的一帧B超图像
以 Puncture 作为单启动项目，读取两个ini配置文件，实时跟踪B超 NDI设备状态，跟随NDI探头(注意 不是超声探头)，实时更新MRI模拟采样探头位置，更新prostate mask

# v1.1
叠加显示mask轮廓线,prostate lesion rectum三种mask，分别以红/绿/蓝色标注轮廓
USBCapturer模块添加USE_LOCAL_US_IMAGE宏，用于在B超探头不在时，使用硬盘图片替代B超图片。
将Debug/Config添加入git库

# v1.2
与mevislab预处理软件mr_processor v2.1联调通过。可以实现对应读取ini配置文件以及raw数据文件。由于SurgicalPlan.ini记录的是数据的绝对路径，如果移动了数据文件夹，仍然需要手动进入ini文件设置绝对路径。

# v1.2.1
修改了AnalyseConfig和SurgicalPlan，MR数据的像素数变量名如m_nSizeX,MR数据的体素大小如m_fVoxelSizeX。
为Puncture\Output\x64\Debug目录添加了若干dll文件，从而可以从git库clone后直接编译生成运行。
出现了mat stackoverflower问题，待解决
帧率目测只有约2fps,需要处理吗？

# v1.3
添加了3个objFile的路径
修正了Calibrate项目的中间目录
添加了.gitignore，不再跟踪不需要的文件
可编译生成，不可运行(缺少SceneGraph.dll)

# v1.3.1
添加了SceneGraph.dll，可git后直接编译生成运行

# v1.4
修复了CUsImageWnd.cpp onPaint()双缓冲引起的内存泄露问题。 AnalyseProcess.cpp引起的stack overflow待修复

# v1.4.1
通过修改Puncture项目的堆栈保留大小，解决了stack overflow问题。现在可以稳定运行(测试了三小时)

# v1.4.2
在PositionManager::SetCurUSPosition()中 设置了长条形NDI探头的初始姿态。以NDI探头作为B超探头姿态(即忽略标定过程)的前提下，测试通过，探头的移动与MASK的变换一致

# v1.4.3
将Coordinate和Matrix类 由AnalyseProcess项目 移动至 ComUtility项目
为3d显示窗口 添加截取平面示意

# v1.5
全部用std::thread替代了c语言的CreateThread()
完成了NDIConfig，从ini文件读取 NDI探头端口号、探头初始姿态、标定变换矩阵。使用了标定矩阵，定性测试通过
添加了fsutility::Attitude类，用四个其次坐标表示姿态，全面替换了NDIOperator::Attitude中欧拉角形式的姿态

# v1.5.1 (未打tag)
release版本生成完成,3d窗口创建过程中 m_p3DShowWnd->CreateWnd() 出现asseration failure debug 版本无问题。很可能是因为父窗口初始化出错

# v1.6
使用qt替换了mfc 原版本功能复现
出现问题  窗口显示的轮廓 红色/蓝色残影

# v1.6.1 
修复了红色/蓝色残影的问题
修复了固定值的内存泄漏
横断面/矢状面/双平面  跟随b超机切换

# v1.6.2
修复了矢状面截取错误的问题 由于矢状面ROI不确定 仍然有偏差
配准前 允许由键盘手动调整MASK位置

# v1.7
为GraphicsView2D读取us图片时 添加gama变换 使得能够清楚看到图像边缘--背景黑色区域有彩色亮带 是因为USBCapturer采集到的8UC3的背景并非全黑。通过转灰度图 阈值筛选>1区域作为roi 解决了该问题
修改了PunctureWindow中的timer定时器  帧率提高了
添加了退出逻辑 在任何情况下退出都不会报错了
显示界面添加了MRI模拟采样的姿态参数


# v1.7.1
发现横断面物理中心始终对应图像下方正中心；矢状面物理中心始终对应图像下方正中心；这个物理偏移量记录在NDIConfig.ini中。在imageSampler计算横断面截图的时候，考虑此变量
更新了USBCapturer.ini，保证截图ROI的横向中心 总是对应中心坐标
更新了超声参数判断逻辑：在每一帧图像判断扫描模式。如果扫描模式改变了，或者医生手动点击更新US参数的按钮，则计算像素物理尺寸

# v1.8
添加了3D显示窗口，以及“导入数据”按钮
导入病人数据，需要用户手动选择数据文件夹。AnalyseProcess.ini SurgicalPlan.ini文件移动至病人文件夹而非程序文件夹

# v1.9
重新添加了Calibrate项目。
CalibrateWindow，主界面
AnatotateWindow,采集一组标定数据后，用来手工标注所有的点(目前是360张图片*9个点)
LocatePhantomDlg，用于定位体模(目前是8个点)
以上的数据，记录为txt文件，待matlab分析计算

# v1.9.1
Calibrate项目的标注模块，新增了功能
1 计算3d图像坐标系物理坐标
2 计算体模坐标系物理坐标

# v1.9.2
修改了Calibrate项目。对LocatePhantomDlg，增添按钮，使得允许一次采集多个点的坐标
相应地修改了NDIOperator项目，允许通过函数返回单个欧拉角形式的探头姿态
标注工具增添了 可以回车确认标注完成单帧图片
定位体模窗口 为定位点坐标添加了序号
将定位体模窗口 设置为关闭时即销毁(DeleteOnClose)。析构时将使用的NDI sensor序号重置
修复了采集模块中的“关闭设备”功能
通过在NDIOperator中停止采集的函数加锁，修复了一个错误
通过在CalibrateWindow::OnTimerCapturer中注释掉QMessageBox一行，修复了一个错误

# v1.10
引入vtk全部的头文件，全部的lib，以及部分dll
更新opencv依赖库至4.1.0(vc15)
添加了ImageProcessor工具类，提供2D图像阈值分割功能
采集US下的前列腺点云
在医生手动配准的基础上，添加了根据US下的前列腺点云 以及 MRI下的前列腺点云，调用vtk的icp算法 优化配准

