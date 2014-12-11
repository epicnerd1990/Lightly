//////////////////////////////////////////////////////////////////////////////
// breezeexceptiondialog.cpp
// -------------------
//
// Copyright (c) 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//////////////////////////////////////////////////////////////////////////////

#include "breezeexceptiondialog.h"
#include "breezeexceptiondialog.moc"
#include "breezedetectwidget.h"

namespace Breeze
{

    //___________________________________________
    ExceptionDialog::ExceptionDialog( QWidget* parent ):
        QDialog( parent )
    {

        setupUi( this );
        connect( buttonBox->button( QDialogButtonBox::Cancel ), SIGNAL(clicked()), this, SLOT(close()) );

        // store checkboxes from ui into list
        m_checkboxes.insert( BorderSize, borderSizeCheckBox );

        // detect window properties
        connect( detectDialogButton, SIGNAL(clicked()), SLOT(selectWindowProperties()) );

        // connections
        connect( exceptionType, SIGNAL(currentIndexChanged(int)), SLOT(updateChanged()) );
        connect( exceptionEditor, SIGNAL(textChanged(QString)), SLOT(updateChanged()) );
        connect( borderSizeComboBox, SIGNAL(currentIndexChanged(int)), SLOT(updateChanged()) );

        for( CheckBoxMap::iterator iter = m_checkboxes.begin(); iter != m_checkboxes.end(); ++iter )
        { connect( iter.value(), SIGNAL(clicked()), SLOT(updateChanged()) ); }

        connect( hideTitleBar, SIGNAL(clicked()), SLOT(updateChanged()) );
    }

    //___________________________________________
    void ExceptionDialog::setException( InternalSettingsPtr exception )
    {

        // store exception internally
        m_exception = exception;

        // type
        exceptionType->setCurrentIndex(m_exception->exceptionType() );
        exceptionEditor->setText( m_exception->exceptionPattern() );
        borderSizeComboBox->setCurrentIndex( m_exception->borderSize() );
        hideTitleBar->setChecked( m_exception->hideTitleBar() );

        // mask
        for( CheckBoxMap::iterator iter = m_checkboxes.begin(); iter != m_checkboxes.end(); ++iter )
        { iter.value()->setChecked( m_exception->mask() & iter.key() ); }

        setChanged( false );

    }

    //___________________________________________
    void ExceptionDialog::save( void )
    {
        m_exception->setExceptionType( exceptionType->currentIndex() );
        m_exception->setExceptionPattern( exceptionEditor->text() );
        m_exception->setBorderSize( borderSizeComboBox->currentIndex() );
        m_exception->setHideTitleBar( hideTitleBar->isChecked() );

        // mask
        unsigned int mask = None;
        for( CheckBoxMap::iterator iter = m_checkboxes.begin(); iter != m_checkboxes.end(); ++iter )
        { if( iter.value()->isChecked() ) mask |= iter.key(); }

        m_exception->setMask( mask );

        setChanged( false );

    }

    //___________________________________________
    void ExceptionDialog::updateChanged( void )
    {
        bool modified( false );
        if( m_exception->exceptionType() != exceptionType->currentIndex() ) modified = true;
        else if( m_exception->exceptionPattern() != exceptionEditor->text() ) modified = true;
        else if( m_exception->borderSize() != borderSizeComboBox->currentIndex() ) modified = true;
        else if( m_exception->hideTitleBar() != hideTitleBar->isChecked() ) modified = true;
        else
        {
            // check mask
            for( CheckBoxMap::iterator iter = m_checkboxes.begin(); iter != m_checkboxes.end(); ++iter )
            {
                if( iter.value()->isChecked() != (bool)( m_exception->mask() & iter.key() ) )
                {
                    modified = true;
                    break;
                }
            }
        }

        setChanged( modified );

    }

    //___________________________________________
    void ExceptionDialog::selectWindowProperties( void )
    {

        // create widget
        if( !m_detectDialog )
        {
            m_detectDialog = new DetectDialog( this );
            connect( m_detectDialog, SIGNAL(detectionDone(bool)), SLOT(readWindowProperties(bool)) );
        }

        m_detectDialog->detect(0);

    }

    //___________________________________________
    void ExceptionDialog::readWindowProperties( bool valid )
    {
        Q_CHECK_PTR( m_detectDialog );
        if( valid )
        {

            // type
            exceptionType->setCurrentIndex( m_detectDialog->exceptionType() );

            // window info
            const KWindowInfo& info( m_detectDialog->windowInfo() );

            switch( m_detectDialog->exceptionType() )
            {

                default:
                case InternalSettings::ExceptionWindowClassName:
                exceptionEditor->setText( QString::fromUtf8( info.windowClassClass() ) );
                break;

                case InternalSettings::ExceptionWindowTitle:
                exceptionEditor->setText( info.name() );
                break;

            }

        }

        delete m_detectDialog;
        m_detectDialog = 0;

    }

}
