// loadprojectdialog.h
#ifndef LOADPROJECTDIALOG_H
#define LOADPROJECTDIALOG_H

#include <QDialog>
#include <QStringList>
#include <QButtonGroup>
#include <QFrame>
#include <QScrollArea>

namespace Ui {
class LoadProjectDialog;
}

/**
 * @class LoadProjectDialog
 * @brief A dialog that allows the user to select a game engine and project directory.
 *
 * This dialog provides a scrollable list of supported game engines, allowing
 * the user to select one visually. It also includes a file browser for selecting
 * the target game project directory.
 */
class LoadProjectDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the dialog.
     * @param availableEngines List of available game engine names (e.g., RPGM, Unity, Ren'Py).
     * @param parent Optional parent widget.
     */
    explicit LoadProjectDialog(const QStringList &availableEngines, QWidget *parent = nullptr);

    /// Destructor
    ~LoadProjectDialog() override;

    /// Returns the name of the currently selected engine (uppercase).
    QString selectedEngine() const;

    /// Returns the user-selected project directory path.
    QString projectPath() const;

protected:
    /// Handles mouse click events for selecting an engine frame.
    //bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    /// Opens a file dialog for browsing the game project directory.
    void browseProjectPath();

    /// Updates the visual selection when an engine radio button is clicked.
    void updateFrameSelection(QAbstractButton *button);

private:
    /* =========================================================================
     *  UI COMPONENTS
     * ========================================================================= */
    Ui::LoadProjectDialog *ui;           ///< Pointer to the auto-generated UI object.
    QButtonGroup *engineButtons;         ///< Group managing all engine selection radio buttons.
    QList<QFrame*> engineFrames;         ///< List of all engine frames for easy style updates.

    /* =========================================================================
     *  UI INITIALIZATION HELPERS
     * ========================================================================= */
    void setupMainLayout(const QStringList &availableEngines);
    void setupPathSection();
    void setupButtonBox();

    /* =========================================================================
     *  ENGINE FRAME HELPERS
     * ========================================================================= */
    QFrame *createEngineFrame(const QString &engine, int index);
    QString defaultEngineFrameStyle() const;
    QPixmap loadEngineIcon(const QString &engine) const;
    QString engineFileTypes(const QString &engine) const;
};

#endif // LOADPROJECTDIALOG_H
