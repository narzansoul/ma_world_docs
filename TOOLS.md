# TOOLS.md - Local Notes

## Git & GitHub Workflow

### Создание репозитория через браузер
1. Открыть GitHub → Нажать "Create repository"
2. Настроить:
   - Repository name
   - Public/Private
   - Add README (опционально)
3. Создать репозиторий

### Создание файла через GitHub Web
1. Открыть репозиторий
2. Нажать "Add file" → "Create new file"
3. Ввести имя файла (например, `index.html`)
4. Ввести содержимое файла
5. Нажать "Commit changes..."
6. Подтвердить коммит

### Клонирование репозитория
```bash
cd ~/Desktop/GitProjects
git clone https://github.com/USERNAME/REPO.git
```

### Синхронизация изменений
```bash
cd REPO
git pull origin main --rebase
# При конфликтах:
git rebase --abort
git reset --hard origin/main
```

### GitHub CLI (gh)
```bash
# Установка
brew install gh

# Авторизация (требуется браузер)
gh auth login

# Настройка git для использования gh
gh auth setup-git
```

## Browser Automation (GitHub)

### Полезные селекторы GitHub:
- Кнопка "Sign in": `[ref=e53]` на главной
- Кнопка "Create repository": `[ref=e75]` на дашборде
- Поле имени репозитория: `[ref=e102]`
- Кнопка создания: `[ref=e175]`
- Меню "Add file": `[ref=e216]`
- "Create new file": `[ref=e377]`
- Поле имени файла: `[ref=e413]`
- Поле содержимого: `[ref=e459]` или `[ref=e544]`
- Кнопка коммита: `[ref=e420]` или `[ref=e670]`

### Device Auth (GitHub CLI)
- URL: `https://github.com/login/device`
- Формат кода: `XXXX-XXXX`
- Таймаут: несколько минут

---

*Последнее обновление: 2026-02-02*
