from pathlib import Path
from sqlalchemy import create_engine, text
from dotenv import load_dotenv

BASE_DIR = Path(__file__).resolve().parent
load_dotenv(BASE_DIR / '.env')

from db import settings  # noqa: E402


def run() -> None:
    sql_path = BASE_DIR / 'migrations.sql'
    sql = sql_path.read_text(encoding='utf-8')
    engine = create_engine(settings.DATABASE_URL, pool_pre_ping=True)
    with engine.begin() as conn:
        conn.exec_driver_sql(sql)
    print('Migrations executed successfully')


if __name__ == '__main__':
    run()


