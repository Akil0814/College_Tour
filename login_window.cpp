#include "login_window.h"
#include "ui_login_window.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>

//For ID_verify
#include <string>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <optional>
//For ID_verify


login_window::login_window(QWidget *parent)
    : QDialog(parent), ui(new Ui::login_window) {


    this->setWindowTitle("Campus Access");
    this->setFixedSize(320, 350);

    // 2. Main Layout with tight spacing
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(30, 30, 30, 30);
    layout->setSpacing(15);

    // Theme Header
    QLabel *header = new QLabel("Member Login");
    header->setStyleSheet("font-size: 22px; font-weight: bold; color: #B41E28;");
    header->setAlignment(Qt::AlignCenter);

    QLineEdit *username = new QLineEdit();
    username->setPlaceholderText("Username");

    QLineEdit *password = new QLineEdit();
    password->setPlaceholderText("Password");
    password->setEchoMode(QLineEdit::Password);

    // 3. The Themed Login Button
    QPushButton *loginBtn = new QPushButton("LOGIN");
    loginBtn->setObjectName("loginBtn");
    loginBtn->setCursor(Qt::PointingHandCursor);

    // Assemble
    layout->addWidget(header);
    layout->addSpacing(10);
    layout->addWidget(username);
    layout->addWidget(password);
    layout->addStretch(1);
    layout->addWidget(loginBtn);

    // 4. Styling to match HomePage (Opacity and Colors)
    this->setStyleSheet(
        "QDialog { background-color: #ffffff; border-radius: 10px; }"
        "QLineEdit { "
        "   padding: 12px; " // Extra padding fixes the text being cut off
        "   border: 2px solid #f2f2f2; "
        "   border-radius: 8px; "
        "   background: #fdfdfd; "
        "   color: #333; "
        "   font-size: 14px; "
        "}"
        "QLineEdit:focus { border: 2px solid #B41E28; }"
        "QPushButton#loginBtn { "
        "   background-color: #B41E28; " // Matches the Homepage Red
        "   color: white; "
        "   border-radius: 20px; "
        "   padding: 12px; "
        "   font-weight: bold; "
        "   font-size: 14px; "
        "}"
        "QPushButton#loginBtn:hover { background-color: #961921; }"
        );

    connect(loginBtn, &QPushButton::clicked, this, &QDialog::accept);
}

login_window::~login_window() {
    delete ui;
}


std::string key_path = R"(C:\CS1D_P1\College_Tour\data\key.dat)";//change this path to local
//user name: cs1d
//password:abc
//------------------------------------------ID_verify----------------------------------------------

static constexpr std::uint32_t magic = 0x44533143u; // 'C''S''1''D' little-endian display
static constexpr std::uint8_t version = 1;
static constexpr std::uint8_t key_mask = 0x5Au;

static std::uint8_t obf_key(std::uint8_t k)
{
    return static_cast<std::uint8_t>(k ^ key_mask);
}

static void xor_inplace(std::string& s, unsigned char key)
{
    for (auto& ch : s)
        ch = static_cast<char>(static_cast<unsigned char>(ch) ^ key);
}

static std::string xor_copy(const std::string& in, std::uint8_t key)
{
    std::string out = in;
    xor_inplace(out, key);
    return out;
}

static bool read_u32(std::ifstream& in, std::uint32_t& v)
{
    in.read(reinterpret_cast<char*>(&v), sizeof(v));
    return static_cast<bool>(in);
}

static bool read_u8(std::ifstream& in, std::uint8_t& v)
{
    int c = in.get();
    if (c == EOF) return false;
    v = static_cast<std::uint8_t>(c);
    return true;
}

static bool read_blob(std::ifstream& in, std::string& s, std::uint32_t len)
{
    s.resize(len);
    in.read(s.data(), static_cast<std::streamsize>(len));
    return static_cast<bool>(in);
}

bool get_key_from_file(const std::string& filename, std::uint8_t& key_out)
{
    std::ifstream in(filename, std::ios::binary);
    if (!in)
    {
        std::cerr << "cannot open: " << filename << "\n";
        return false;
    }

    std::uint32_t file_magic = 0;
    if (!read_u32(in, file_magic))
    {
        std::cerr << "read magic failed\n";
        return false;
    }
    if (file_magic != magic)
    {
        std::cerr << "magic mismatch\n";
        return false;
    }

    std::uint8_t file_version = 0;
    if (!read_u8(in, file_version))
    {
        std::cerr << "read version failed\n";
        return false;
    }
    if (file_version != version)
    {
        std::cerr << "version mismatch\n";
        return false;
    }

    in.seekg(0, std::ios::end);
    auto size = in.tellg();
    if (size < 1)
    {
        std::cerr << "file empty\n";
        return false;
    }

    in.seekg(-1, std::ios::end);
    std::uint8_t key_obf_byte = 0;
    if (!read_u8(in, key_obf_byte))
    {
        std::cerr << "read tail key failed\n";
        return false;
    }

    key_out = static_cast<std::uint8_t>(key_obf_byte ^ key_mask);
    return true;
}

bool id_verify(std::string i_user_name,std::string i_password)
{
    if (i_user_name.empty() || i_password.empty())
        return false;

    std::uint8_t key = 0;
    if (!get_key_from_file(key_path, key))
    {
        std::cerr << "get_key_from_file failed\n";
        return false;
    }

    std::ifstream in(key_path, std::ios::binary);
    if (!in)
    {
        std::cerr << "open key.dat failed\n";
        return false;
    }

    std::uint32_t file_magic = 0;
    if (!read_u32(in, file_magic)) return false;
    if (file_magic != magic)
    {
        std::cerr << "bad file magic\n";
        return false;
    }

    std::uint8_t file_version = 0;
    if (!read_u8(in, file_version)) return false;
    if (file_version != version)
    {
        std::cerr << "unsupported version\n";
        return false;
    }

    std::uint32_t u_len = 0, p_len = 0;

    if (!read_u32(in, u_len))
        return false;
    std::string u_enc_stored;
    if (!read_blob(in, u_enc_stored, u_len))
        return false;

    if (!read_u32(in, p_len))
        return false;
    std::string p_enc_stored;
    if (!read_blob(in, p_enc_stored, p_len))
        return false;

    const std::string u_enc_input = xor_copy(i_user_name, key);
    const std::string p_enc_input = xor_copy(i_password, key);

    return (u_enc_input == u_enc_stored) && (p_enc_input == p_enc_stored);
}

//------------------------------------------ID_verify----------------------------------------------


