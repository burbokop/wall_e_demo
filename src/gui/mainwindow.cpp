#include "mainwindow.h"


#include <src/km2/km2.h>

//TreeWidget *MainWindow::treeWidget() const {
//    return m_treeWidget;
//}

void MainWindow::updateTree() {
    km2_flags flags;
  //  if(verboseChackBox->isChecked()) {
  //      flags.push_back(km2_verbose);
  //  }
  //
  //  if(treeModeChackBox->isChecked()) {
  //      flags.push_back(km2_only_tree);
  //  }
  //
  //  auto cresult = km2_compile(textEdit->toPlainText().toStdString(), flags);
  //
  //  tokensArea->setText(QString::fromStdString(klex_to_string(cresult.tokens)));
  //  rulesArea->setText(QString::fromStdString(cresult.rules));
  //  m_treeWidget->displayTree(cresult.tree);
}

MainWindow::MainWindow(QObject *parent) : QObject(parent) {
    //QHBoxLayout *layout = new QHBoxLayout(this);
    //layout->addWidget(treeModeChackBox);
    //layout->addWidget(verboseChackBox);
    //layout->addWidget(textEdit);
    //layout->addWidget(m_treeWidget);
    //layout->addWidget(rulesArea);
    //layout->addWidget(tokensArea);
    //setLayout(layout);
    //
    //connect(textEdit, &QTextEdit::textChanged, this, &MainWindow::updateTree);
    //connect(treeModeChackBox, &QCheckBox::stateChanged, this, &MainWindow::updateTree);
    //connect(verboseChackBox, &QCheckBox::stateChanged, this, &MainWindow::updateTree);
}
