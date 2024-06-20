#include <QApplication>
#include <QCoreApplication>
#include <QtSql>
#include <QWidget>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QPushButton>
#include <QMessageBox>
#include <QLabel>
#include <QStyle>
#include <QScreen>
#include <QRandomGenerator>
#include <QDateTime>
#include <QTextCodec> // qt5

// Global Variables
int numberOfQuestions = 10;
int score = 0;


QList<QSqlRecord> allQuestionsList;
QList<QSqlRecord> selectedQuestionsList;
QList<int> randomNumbers;
QList<int> correctAnswers;
// end of Global Variables

int getDataFromDb(){
    // Create a QSqlDatabase object and add the SQLite database file
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("sqlite.db");

    // Open the database
    if (!db.open()) {
        qDebug() << "Error: Unable to open database";
        return 1;
    }

    // Retrieve all tables in the database
    QStringList tables = db.tables();
    foreach (const QString &table, tables) {
        qDebug() << "Table name:" << table;

        // Retrieve all data from the table
        QSqlQuery query("SELECT * FROM " + table);
        while (query.next()) {
            QSqlRecord record = query.record();
            allQuestionsList.append(record);
        }
    }

    // Close the database
    db.close();
    return 0;
}

void addQuestionToWidget(int item, QWidget *page){
    QVBoxLayout *layout = new QVBoxLayout;

    QLabel *question = new QLabel(page);
    question->setText(QString::number(item + 1) + ". What is the Meaning of " + selectedQuestionsList.value(item).value(2).toString() + " ?");
    layout->addWidget(question);

    QRadioButton *option1 = new QRadioButton(page);
    QRadioButton *option2 = new QRadioButton(page);
    QRadioButton *option3 = new QRadioButton(page);
    QRadioButton *option4 = new QRadioButton(page);

    if(correctAnswers.value(item) == 1) {
        option1->setText(selectedQuestionsList.value(item).value(3).toString());
        option1->setProperty("isCorrect", "true");
    } else {
        option1->setText(allQuestionsList.value(QRandomGenerator::global()->bounded(1, allQuestionsList.size() + 1)).value(3).toString());
    }

    if(correctAnswers.value(item) == 2) {
        option2->setText(selectedQuestionsList.value(item).value(3).toString());
        option2->setProperty("isCorrect", "true");
    } else {
        option2->setText(allQuestionsList.value(QRandomGenerator::global()->bounded(1, allQuestionsList.size() + 1)).value(3).toString());
    }

    if(correctAnswers.value(item) == 3) {
        option3->setText(selectedQuestionsList.value(item).value(3).toString());
        option3->setProperty("isCorrect", "true");
    } else {
        option3->setText(allQuestionsList.value(QRandomGenerator::global()->bounded(1, allQuestionsList.size() + 1)).value(3).toString());
    }

    if(correctAnswers.value(item) == 4) {
        option4->setText(selectedQuestionsList.value(item).value(3).toString());
        option4->setProperty("isCorrect", "true");
    } else {
        option4->setText(allQuestionsList.value(QRandomGenerator::global()->bounded(1, allQuestionsList.size() + 1)).value(3).toString());
    }

    layout->addWidget(option1);
    layout->addWidget(option2);
    layout->addWidget(option3);
    layout->addWidget(option4);

    QPushButton *submitButton = new QPushButton(page);
    submitButton->setObjectName("Submit" + QString::number(item));
    submitButton->setText("Submit");
    layout->addWidget(submitButton);

    QObject *receiverObject = new QObject();

    QObject::connect(submitButton, &QPushButton::clicked, receiverObject, [=]() {
        if (option1->isChecked() && correctAnswers.value(item) == 1) {
            score += 1;
        } else if (option2->isChecked() && correctAnswers.value(item) == 2) {
            score += 1;
        } else if (option3->isChecked() && correctAnswers.value(item) == 3) {
            score += 1;
        } else if (option4->isChecked() && correctAnswers.value(item) == 4) {
            score += 1;
        }
    });

    // layout->addWidget(page);
    page->setLayout(layout);
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);


    // Seed the random number generator
    QRandomGenerator::global()->generate();

    if (getDataFromDb()){
        return 1;
    }

    QWidget window;
    window.setWindowTitle("504 Vocab Quiz");

    QStackedWidget *stackedWidget = new QStackedWidget;
    stackedWidget->setCurrentIndex(0);

    // Set window size and center it on the screen
    int windowWidth = 400;
    int windowHeight = 300;
    window.resize(windowWidth, windowHeight);
    window.setGeometry(windowWidth, windowHeight, windowWidth, windowHeight - 100);

    // Generate 10 random numbers between 1 and allQuestionsList.size
    for (int i = 0; i < numberOfQuestions; ++i) {
        int randomNumber = QRandomGenerator::global()->bounded(1, allQuestionsList.size() + 1);
        randomNumbers.append(randomNumber);
    }

    // Generate 10 random numbers between 1 and 4
    for (int i = 0; i < numberOfQuestions; ++i) {
        int randomNumber = QRandomGenerator::global()->bounded(1, 5);
        correctAnswers.append(randomNumber);
    }

    // store selected random questions into selectedQuestionsList
    for (int it = 0; it < randomNumbers.size(); ++it) {
        selectedQuestionsList.append(allQuestionsList.value(randomNumbers.value(it)));
        allQuestionsList.removeAt(randomNumbers.value(it));
    }


    for (int item = 0; item < selectedQuestionsList.size(); ++item) {
        QWidget *page = new QWidget;

        addQuestionToWidget(item, page);

        stackedWidget->addWidget(page);
    }

    QPushButton *nextButton = new QPushButton(&window);
    nextButton->setText("Next");

    QObject *tmpReceiverObject = new QObject();

    // switches to the next page
    QObject::connect(nextButton, &QPushButton::clicked, tmpReceiverObject, [=]() {
        int nextPageIndex = qMin((stackedWidget->currentIndex() + 1) , stackedWidget->count());
        stackedWidget->setCurrentIndex(nextPageIndex);

        if((stackedWidget->currentIndex() + 1) == numberOfQuestions){
            // score page
            QWidget *page = new QWidget;

            QVBoxLayout *scoreLayout = new QVBoxLayout;

            QLabel *scoreText = new QLabel(page);
            scoreText->setText("Your Score is : " + QString::number(score));
            scoreLayout->addWidget(scoreText);

            stackedWidget->addWidget(page);
        }
    });

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(stackedWidget);
    layout->addWidget(nextButton);

    window.setLayout(layout);

    window.show();

    return app.exec();
}
