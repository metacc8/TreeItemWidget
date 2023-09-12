#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "treemodel.h"
#include "treeitem.h"
#include "showmodelrecord.h"
#include "showrecorddata.h"
#include <vector>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    treeView = new TreeItemWidget(this);
    treeView->setFocusPolicy(Qt::NoFocus);                         //去掉鼠标移到单元格上时的虚线框
    setCentralWidget(treeView);

    setModel();
    //setFastModel();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setModel()
{
    const char* headers[]={"name","sex","age","phone"};

    TreeModel* model = new TreeModel( headers );
    treeView->setupDataModel(model);

    TreeItem* root = model->returnRootItem();
    // 初始化数据，5个省，每个省5人
    int provinceCount = 100;
    int personCount = 10000;


    TreeItem* province;
    TreeItem* person;

    for(int i = 0; i < provinceCount; i++)
    {


        province = new TreeItem(root);
        province->setData(0, STRING("Province%1").arg(i) );

        root->addChild(province);

        for(int j = 0; j < personCount; j++)
        {

            person = new TreeItem(province);
            person->setData(0, STRING("name%1").arg(i) );
            person->setData(1, STRING("man") );
            person->setData(2, QVariant(25) );
            person->setData(3, STRING("123456789") );

            province->addChild(person);
        }

    }

    //treeView->updateDisplay();

}


void MainWindow::setFastModel()
{
    const char* headers[]={"name","sex","age","phone"};

    TreeModel* model = new TreeModel( headers );
    treeView->setupDataModel(model);
    std::vector<ShowRecordData> showData;
    ShowRecordData s;
    s.setData(0,"Tom");
    s.setData(1,"man");
    s.setData(2,"25");
    s.setData(3,"123456789");
    showData.push_back(s);

    s.setData(0,"Tom1");
    s.setData(1,"man");
    s.setData(2,"25");
    s.setData(3,"123456789");
    showData.push_back(s);

    ShowModelRecord* model_record = new ShowModelRecord();
    model_record->vecRecord.swap(showData);

    model->setModelRecord(model_record);

    //treeView->updateDisplay();

}
