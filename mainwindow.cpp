#include <iostream>
#include <exception>
#include <qlabel.h>
#include <mainwindow.h>
#include <parseexception.h>
#include <ui_mainwindow.h>

using namespace std;

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
	ui(new Ui::MainWindow),
	fdialog(),
	csv(NULL),
	rows() {
	ui->setupUi(this);

	fdialog.setFilter(QDir::AllDirs | QDir::Files | QDir::NoDot | QDir::NoDotDot);
	fdialog.setNameFilter("Spreadsheet (*.csv)");

	connect(&fdialog, &QFileDialog::finished, this, &MainWindow::fileSelected);
	connect(ui->open_file_button, &QPushButton::pressed, this, &MainWindow::openFilePressed);
}

MainWindow::~MainWindow() {
    delete ui;
	if (csv != NULL) {
		csv->close();
		delete csv;
	}
	for (int i = 0; i < rows.length(); ++i) {
		delete rows[i];
	}
}

void MainWindow::fileSelected(int status) {
	if (status == QDialog::Accepted) {
		QFileInfo info(fdialog.selectedFiles()[0]);
		if (info.isReadable()) {
			if (csv != NULL) {
				csv->close();
				delete csv;
			}
			csv = new CSVFile(info.filePath());
			if (!csv->open()) {
				showMessage("Failed to open file.  Please try again.");
			} else {
				showMessage("opened file");
			}

			QList<QString> header;
			QList<QString> row;
			bool more = false;
			try {
				more = csv->readRow(header);

				while (more) {
					row.clear();
					more = csv->readRow(row);
					rows.append(new Row(header, row));
				}
			} catch (ParseException& e) {
				showMessage("There was an issue with the CSV file (" + e.message() + "): " + e.context());
			} catch (exception& e) {
				showMessage("An unexpected error occured: " + QString(e.what()));
			}
		} else {
			showMessage("The file you selected isn't readable.  Please select another file.");
		}
	} else {
		showMessage("Please select a spreadsheet.");
	}
}

void MainWindow::showMessage(const QString& message) {
	ui->message_label->setText(message);
}

void MainWindow::openFilePressed() {
	fdialog.show();
}
