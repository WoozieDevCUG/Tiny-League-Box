-- Users table
CREATE TABLE IF NOT EXISTS users (
    id SERIAL PRIMARY KEY,
    username VARCHAR(64) UNIQUE NOT NULL,
    password_hash VARCHAR(256) NOT NULL,
    is_admin BOOLEAN NOT NULL DEFAULT FALSE,
    level INTEGER NOT NULL DEFAULT 1,
    avatar_url VARCHAR(512),
    created_at TIMESTAMP NOT NULL DEFAULT NOW(),
    updated_at TIMESTAMP NOT NULL DEFAULT NOW()
);

-- Heroes basic table
CREATE TABLE IF NOT EXISTS heroes (
    id INTEGER PRIMARY KEY,
    title VARCHAR(128) NOT NULL,
    name VARCHAR(128) NOT NULL,
    role_route VARCHAR(16) NOT NULL,  -- top|mid|bottom|jungle|support
    role_class VARCHAR(16) NOT NULL,  -- tank|fighter|assassin|marksman|mage
    square_image_url TEXT
);

-- Hero details table
CREATE TABLE IF NOT EXISTS hero_details (
    hero_id INTEGER PRIMARY KEY REFERENCES heroes(id) ON DELETE CASCADE,
    cover_image_url TEXT,
    story TEXT,
    type INTEGER,
    difficulty INTEGER,
    radar_damage INTEGER,
    radar_utility INTEGER,
    radar_mobility INTEGER,
    radar_control INTEGER,
    radar_toughness INTEGER,
    -- 技能名称与描述
    passive_name TEXT,
    passive_result TEXT,
    q_name TEXT,
    q_result TEXT,
    w_name TEXT,
    w_result TEXT,
    e_name TEXT,
    e_result TEXT,
    r_name TEXT,
    r_result TEXT,
    -- 技能图标
    passive_image TEXT,
    q_image TEXT,
    w_image TEXT,
    e_image TEXT,
    r_image TEXT
);

-- Hero stats per level (1..18)
CREATE TABLE IF NOT EXISTS hero_stats (
    hero_id INTEGER REFERENCES heroes(id) ON DELETE CASCADE,
    level INTEGER CHECK (level >= 1 AND level <= 18),
    attack INTEGER,
    attack_speed INTEGER,
    attack_range INTEGER,
    move_speed INTEGER,
    health INTEGER,
    mana INTEGER,
    armor INTEGER,
    mr INTEGER,
    PRIMARY KEY (hero_id, level)
);

-- Hero skills
CREATE TABLE IF NOT EXISTS hero_skills (
    id SERIAL PRIMARY KEY,
    hero_id INTEGER REFERENCES heroes(id) ON DELETE CASCADE,
    key VARCHAR(8) NOT NULL, -- Passive, Q, W, E, R
    name VARCHAR(128) NOT NULL,
    description TEXT,
    icon_url TEXT
);

-- Hero skins table (composite key: hero_id + skin_index)
DO $$
BEGIN
    IF NOT EXISTS (
        SELECT 1 FROM information_schema.tables WHERE table_name='hero_skins'
    ) THEN
        CREATE TABLE hero_skins (
            hero_id INTEGER NOT NULL REFERENCES heroes(id) ON DELETE CASCADE,
            skin_index INTEGER NOT NULL,
            skin_name VARCHAR(128) NOT NULL,
            skin_image TEXT NOT NULL,
            PRIMARY KEY (hero_id, skin_index)
        );
    END IF;
END
$$;

-- Handle legacy column name 'rating' vs 'stars'
DO $$
DECLARE
    has_rating BOOLEAN;
    has_stars BOOLEAN;
    has_check BOOLEAN;
BEGIN
    SELECT EXISTS (
        SELECT 1 FROM information_schema.columns
        WHERE table_name='hero_ratings' AND column_name='rating'
    ) INTO has_rating;

    IF has_rating THEN
        SELECT EXISTS (
            SELECT 1 FROM information_schema.columns
            WHERE table_name='hero_ratings' AND column_name='stars'
        ) INTO has_stars;

        IF NOT has_stars THEN
            EXECUTE 'ALTER TABLE hero_ratings ADD COLUMN stars INTEGER';
        END IF;

        EXECUTE 'UPDATE hero_ratings SET stars = rating WHERE stars IS NULL';

        BEGIN
            EXECUTE 'ALTER TABLE hero_ratings ALTER COLUMN stars SET NOT NULL';
        EXCEPTION WHEN others THEN
            NULL;
        END;

        SELECT EXISTS (
          SELECT 1
          FROM pg_constraint c
          JOIN pg_class t ON c.conrelid = t.oid
          WHERE t.relname = 'hero_ratings' AND c.conname = 'chk_hero_ratings_stars'
        ) INTO has_check;
        IF NOT has_check THEN
            EXECUTE 'ALTER TABLE hero_ratings ADD CONSTRAINT chk_hero_ratings_stars CHECK (stars >= 0 AND stars <= 5)';
        END IF;

        BEGIN
            EXECUTE 'ALTER TABLE hero_ratings ALTER COLUMN rating DROP NOT NULL';
        EXCEPTION WHEN undefined_column THEN
            NULL;
        END;
    END IF;
END
$$;

-- Ensure hero_stats.attack_speed is INTEGER (match ORM)
DO $$
DECLARE
    dtype TEXT;
BEGIN
    SELECT data_type FROM information_schema.columns
    WHERE table_name = 'hero_stats' AND column_name = 'attack_speed'
    INTO dtype;

    IF dtype IS NOT NULL AND dtype <> 'integer' THEN
        ALTER TABLE hero_stats
        ALTER COLUMN attack_speed TYPE INTEGER USING COALESCE(ROUND(attack_speed)::integer, 0);
    END IF;
END
$$;

-- Hero ratings table for interactive scoring
DO $$
BEGIN
    IF NOT EXISTS (
        SELECT 1 FROM information_schema.tables WHERE table_name='hero_ratings'
    ) THEN
        CREATE TABLE hero_ratings (
            id SERIAL PRIMARY KEY,
            user_id INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE,
            hero_id INTEGER NOT NULL REFERENCES heroes(id) ON DELETE CASCADE,
            stars INTEGER NOT NULL CHECK (stars >= 0 AND stars <= 5),
            comment VARCHAR(50),
            created_at TIMESTAMP NOT NULL DEFAULT NOW()
        );
        CREATE INDEX idx_hero_ratings_hero_id ON hero_ratings(hero_id);
        CREATE UNIQUE INDEX uq_hero_ratings_user_hero ON hero_ratings(user_id, hero_id);
    END IF;
END
$$;

-- Ensure hero_ratings required columns exist (compat with older schemas)
DO $$
BEGIN
    IF EXISTS (SELECT 1 FROM information_schema.tables WHERE table_name='hero_ratings') THEN
        -- topic column support (topic like: top_tank_strength)
        IF NOT EXISTS (SELECT 1 FROM information_schema.columns WHERE table_name='hero_ratings' AND column_name='topic') THEN
            ALTER TABLE hero_ratings ADD COLUMN topic VARCHAR(64) NOT NULL DEFAULT 'default_topic';
        END IF;
        IF NOT EXISTS (SELECT 1 FROM information_schema.columns WHERE table_name='hero_ratings' AND column_name='stars') THEN
            ALTER TABLE hero_ratings ADD COLUMN stars INTEGER NOT NULL DEFAULT 0;
            ALTER TABLE hero_ratings ADD CONSTRAINT chk_hero_ratings_stars CHECK (stars >= 0 AND stars <= 5);
        END IF;
        IF NOT EXISTS (SELECT 1 FROM information_schema.columns WHERE table_name='hero_ratings' AND column_name='comment') THEN
            ALTER TABLE hero_ratings ADD COLUMN comment VARCHAR(50);
        END IF;
        IF NOT EXISTS (SELECT 1 FROM information_schema.columns WHERE table_name='hero_ratings' AND column_name='created_at') THEN
            ALTER TABLE hero_ratings ADD COLUMN created_at TIMESTAMP NOT NULL DEFAULT NOW();
        END IF;
        -- Unique index to avoid duplicate ratings per (user, hero)
        -- Prefer including topic if column exists
        IF EXISTS (SELECT 1 FROM information_schema.columns WHERE table_name='hero_ratings' AND column_name='topic') THEN
            IF NOT EXISTS (
                SELECT 1 FROM pg_indexes WHERE tablename='hero_ratings' AND indexname='uq_hero_ratings_user_hero_topic'
            ) THEN
                CREATE UNIQUE INDEX uq_hero_ratings_user_hero_topic ON hero_ratings(user_id, hero_id, topic);
            END IF;
        ELSE
            IF NOT EXISTS (
                SELECT 1 FROM pg_indexes WHERE tablename='hero_ratings' AND indexname='uq_hero_ratings_user_hero'
            ) THEN
                CREATE UNIQUE INDEX uq_hero_ratings_user_hero ON hero_ratings(user_id, hero_id);
            END IF;
        END IF;
        IF NOT EXISTS (
            SELECT 1 FROM pg_indexes WHERE tablename='hero_ratings' AND indexname='idx_hero_ratings_hero_id'
        ) THEN
            CREATE INDEX idx_hero_ratings_hero_id ON hero_ratings(hero_id);
        END IF;
    END IF;
END
$$;

-- If topic column exists, drop legacy unique index without topic and ensure per-topic uniqueness
DO $$
BEGIN
    IF EXISTS (SELECT 1 FROM information_schema.columns WHERE table_name='hero_ratings' AND column_name='topic') THEN
        IF EXISTS (
            SELECT 1 FROM pg_indexes 
            WHERE tablename='hero_ratings' AND indexname='uq_hero_ratings_user_hero'
        ) THEN
            DROP INDEX uq_hero_ratings_user_hero;
        END IF;
        IF NOT EXISTS (
            SELECT 1 FROM pg_indexes 
            WHERE tablename='hero_ratings' AND indexname='uq_hero_ratings_user_hero_topic'
        ) THEN
            CREATE UNIQUE INDEX uq_hero_ratings_user_hero_topic ON hero_ratings(user_id, hero_id, topic);
        END IF;
    END IF;
END
$$;

-- Seed one example rating if table exists and empty for a known hero
DO $$
DECLARE
    any_user_id INTEGER;
    any_hero_id INTEGER;
    existing_count INTEGER;
BEGIN
    SELECT COUNT(*) INTO existing_count FROM information_schema.tables WHERE table_name='hero_ratings';
    IF existing_count > 0 THEN
        -- pick a hero: prefer id=2 (加里奥), else pick the first hero
        SELECT id INTO any_hero_id FROM heroes WHERE id = 2 LIMIT 1;
        IF any_hero_id IS NULL THEN
            SELECT id INTO any_hero_id FROM heroes ORDER BY id ASC LIMIT 1;
        END IF;

        -- ensure a user exists
        SELECT id INTO any_user_id FROM users ORDER BY id ASC LIMIT 1;
        IF any_user_id IS NULL THEN
            INSERT INTO users (username, password_hash, is_admin, level)
            VALUES ('demo', '$2b$12$abcdefghijklmnopqrstuvwx1u2v3w4x5y6z7A8B9C0D1E2F3G4H5', FALSE, 1) RETURNING id INTO any_user_id;
            -- Note: password hash is placeholder; demo user is for seed only
        END IF;

        -- insert sample if not exists
        IF any_hero_id IS NOT NULL AND any_user_id IS NOT NULL THEN
            IF EXISTS (SELECT 1 FROM information_schema.columns WHERE table_name='hero_ratings' AND column_name='topic') THEN
                IF NOT EXISTS (
                    SELECT 1 FROM hero_ratings WHERE user_id = any_user_id AND hero_id = any_hero_id AND topic = 'top_tank_strength'
                ) THEN
                    IF EXISTS (SELECT 1 FROM information_schema.columns WHERE table_name='hero_ratings' AND column_name='rating') THEN
                        INSERT INTO hero_ratings (user_id, hero_id, topic, stars, comment, rating)
                        VALUES (any_user_id, any_hero_id, 'top_tank_strength', 4, '坦度在线', 4);
                    ELSE
                        INSERT INTO hero_ratings (user_id, hero_id, topic, stars, comment)
                        VALUES (any_user_id, any_hero_id, 'top_tank_strength', 4, '坦度在线');
                    END IF;
                END IF;
            ELSE
                IF NOT EXISTS (
                    SELECT 1 FROM hero_ratings WHERE user_id = any_user_id AND hero_id = any_hero_id
                ) THEN
                    IF EXISTS (SELECT 1 FROM information_schema.columns WHERE table_name='hero_ratings' AND column_name='rating') THEN
                        INSERT INTO hero_ratings (user_id, hero_id, stars, comment, rating)
                        VALUES (any_user_id, any_hero_id, 4, '坦度在线', 4);
                    ELSE
                        INSERT INTO hero_ratings (user_id, hero_id, stars, comment)
                        VALUES (any_user_id, any_hero_id, 4, '坦度在线');
                    END IF;
                END IF;
            END IF;
        END IF;
    END IF;
END
$$;

-- Add skill name/result columns to hero_details if not exists
DO $$
BEGIN
    IF NOT EXISTS (SELECT 1 FROM information_schema.columns 
                   WHERE table_name = 'hero_details' AND column_name = 'passive_name') THEN
        ALTER TABLE hero_details ADD COLUMN passive_name TEXT;
    END IF;
    IF NOT EXISTS (SELECT 1 FROM information_schema.columns 
                   WHERE table_name = 'hero_details' AND column_name = 'passive_result') THEN
        ALTER TABLE hero_details ADD COLUMN passive_result TEXT;
    END IF;
    IF NOT EXISTS (SELECT 1 FROM information_schema.columns 
                   WHERE table_name = 'hero_details' AND column_name = 'q_name') THEN
        ALTER TABLE hero_details ADD COLUMN q_name TEXT;
    END IF;
    IF NOT EXISTS (SELECT 1 FROM information_schema.columns 
                   WHERE table_name = 'hero_details' AND column_name = 'q_result') THEN
        ALTER TABLE hero_details ADD COLUMN q_result TEXT;
    END IF;
    IF NOT EXISTS (SELECT 1 FROM information_schema.columns 
                   WHERE table_name = 'hero_details' AND column_name = 'w_name') THEN
        ALTER TABLE hero_details ADD COLUMN w_name TEXT;
    END IF;
    IF NOT EXISTS (SELECT 1 FROM information_schema.columns 
                   WHERE table_name = 'hero_details' AND column_name = 'w_result') THEN
        ALTER TABLE hero_details ADD COLUMN w_result TEXT;
    END IF;
    IF NOT EXISTS (SELECT 1 FROM information_schema.columns 
                   WHERE table_name = 'hero_details' AND column_name = 'e_name') THEN
        ALTER TABLE hero_details ADD COLUMN e_name TEXT;
    END IF;
    IF NOT EXISTS (SELECT 1 FROM information_schema.columns 
                   WHERE table_name = 'hero_details' AND column_name = 'e_result') THEN
        ALTER TABLE hero_details ADD COLUMN e_result TEXT;
    END IF;
    IF NOT EXISTS (SELECT 1 FROM information_schema.columns 
                   WHERE table_name = 'hero_details' AND column_name = 'r_name') THEN
        ALTER TABLE hero_details ADD COLUMN r_name TEXT;
    END IF;
    IF NOT EXISTS (SELECT 1 FROM information_schema.columns 
                   WHERE table_name = 'hero_details' AND column_name = 'r_result') THEN
        ALTER TABLE hero_details ADD COLUMN r_result TEXT;
    END IF;
END
$$;

-- If old schema with id column exists, migrate it
DO $$
DECLARE
    col_exists BOOLEAN;
BEGIN
    SELECT EXISTS (
        SELECT 1 FROM information_schema.columns 
        WHERE table_name='hero_skins' AND column_name='id'
    ) INTO col_exists;

    IF col_exists THEN
        -- Create new table structure
        CREATE TABLE IF NOT EXISTS hero_skins_new (
            hero_id INTEGER NOT NULL REFERENCES heroes(id) ON DELETE CASCADE,
            skin_index INTEGER NOT NULL,
            skin_name VARCHAR(128) NOT NULL,
            skin_image TEXT NOT NULL,
            PRIMARY KEY (hero_id, skin_index)
        );

        -- Migrate data: assign skin_index by ordering within each hero_id
        INSERT INTO hero_skins_new (hero_id, skin_index, skin_name, skin_image)
        SELECT hero_id,
               ROW_NUMBER() OVER (PARTITION BY hero_id ORDER BY id) AS skin_index,
               skin_name,
               skin_image
        FROM hero_skins;

        DROP TABLE hero_skins;
        ALTER TABLE hero_skins_new RENAME TO hero_skins;
    END IF;
END
$$;

-- Index to speed up queries by hero_id
DO $$
BEGIN
    -- Primary key includes hero_id; extra index usually unnecessary
END
$$;

-- Seed minimal heroes if empty
INSERT INTO heroes (id, title, name, role_route, role_class, square_image_url)
SELECT 1, '青钢影', '卡密尔', 'top', 'fighter', NULL
WHERE NOT EXISTS (SELECT 1 FROM heroes WHERE id = 1);

INSERT INTO heroes (id, title, name, role_route, role_class, square_image_url)
SELECT 2, '正义巨像', '加里奥', 'mid', 'tank', NULL
WHERE NOT EXISTS (SELECT 1 FROM heroes WHERE id = 2);

-- Add avatar_url column to existing users table if not exists
DO $$
BEGIN
    IF NOT EXISTS (SELECT 1 FROM information_schema.columns 
                   WHERE table_name = 'users' AND column_name = 'avatar_url') THEN
        ALTER TABLE users ADD COLUMN avatar_url VARCHAR(512);
    END IF;
END
$$; 

-- Add skill image columns to hero_details if not exists
DO $$
BEGIN
    IF NOT EXISTS (SELECT 1 FROM information_schema.columns 
                   WHERE table_name = 'hero_details' AND column_name = 'passive_image') THEN
        ALTER TABLE hero_details ADD COLUMN passive_image TEXT;
    END IF;
    IF NOT EXISTS (SELECT 1 FROM information_schema.columns 
                   WHERE table_name = 'hero_details' AND column_name = 'q_image') THEN
        ALTER TABLE hero_details ADD COLUMN q_image TEXT;
    END IF;
    IF NOT EXISTS (SELECT 1 FROM information_schema.columns 
                   WHERE table_name = 'hero_details' AND column_name = 'w_image') THEN
        ALTER TABLE hero_details ADD COLUMN w_image TEXT;
    END IF;
    IF NOT EXISTS (SELECT 1 FROM information_schema.columns 
                   WHERE table_name = 'hero_details' AND column_name = 'e_image') THEN
        ALTER TABLE hero_details ADD COLUMN e_image TEXT;
    END IF;
    IF NOT EXISTS (SELECT 1 FROM information_schema.columns 
                   WHERE table_name = 'hero_details' AND column_name = 'r_image') THEN
        ALTER TABLE hero_details ADD COLUMN r_image TEXT;
    END IF;
END
$$;

-- Add type/difficulty columns to hero_details if not exists (compat safety)
DO $$
BEGIN
    IF NOT EXISTS (
        SELECT 1 FROM information_schema.columns 
        WHERE table_name = 'hero_details' AND column_name = 'type') THEN
        ALTER TABLE hero_details ADD COLUMN type INTEGER;
    END IF;
    IF NOT EXISTS (
        SELECT 1 FROM information_schema.columns 
        WHERE table_name = 'hero_details' AND column_name = 'difficulty') THEN
        ALTER TABLE hero_details ADD COLUMN difficulty INTEGER;
    END IF;
END
$$;